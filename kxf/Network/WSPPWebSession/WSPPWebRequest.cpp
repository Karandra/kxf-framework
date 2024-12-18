#include "kxf-pch.h"
#include "WSPPWebRequest.h"
#include "WSPPWebSession.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/MemoryStream.h"
#include "kxf/IO/StreamReaderWriter.h"

namespace kxf
{
	std::shared_ptr<WSPPWebRequest::TConnection> WSPPWebRequest::DoCreateConnection()
	{
		// Set user-agent
		auto userAgent = WebRequestHeader::UserAgent({});
		for (const WebRequestHeader& header: m_RequestHeaders)
		{
			if (header.IsSameAs(userAgent))
			{
				m_Client.set_user_agent(header.GetValue().ToUTF8());
				break;
			}
		}

		auto GetConnection = [&](const URI& uri) -> std::shared_ptr<TConnection>
		{
			TErrorCode errorCode;
			if (auto connection = m_Client.get_connection(uri.BuildURI().ToUTF8(), errorCode))
			{
				return connection;
			}
			return nullptr;
		};
		if (m_Session.m_BaseURI && m_URI.IsReference())
		{
			// Resolve with base URI
			return GetConnection(m_Session.ResolveURI(m_URI).BuildURI());
		}
		else
		{
			// Use the URI as is
			return GetConnection(m_URI.BuildURI());
		}
	}
	void WSPPWebRequest::DoRegisterHandlers()
	{
		m_Client.set_open_handler([this](TConnectionHandle handle)
		{
			m_ConnectionHandle = handle;

			// Switch state to active
			UpdateResponseData();
			ChangeStateAndNotify(WebRequestState::Active);

			// Make the response object
			m_Response.emplace(*this);

			// Send data
			DoPrepareSendData();

			// Process response headers
			for (auto&& [name, value]: m_Connection->get_request().get_headers())
			{
				WebRequestHeader header(String::FromUTF8(name), String::FromUTF8(value));
				WSPPWebSession::SetHeader(m_ResponseHeaders, header, WebRequestHeaderFlag::Add|WebRequestHeaderFlag::CoalesceSemicolon);

				WebRequestEvent event(LockRef(), m_State, std::move(header));
				NotifyEvent(WebRequestEvent::EvtHeaderReceived, event);
			}
		});
		m_Client.set_fail_handler([this](TConnectionHandle handle)
		{
			UpdateResponseData();
			ChangeStateAndNotify(WebRequestState::Failed, m_Connection->get_response_code(), m_Connection->get_response_msg());
		});
		m_Client.set_close_handler([this](TConnectionHandle handle)
		{
			UpdateResponseData();
			ChangeStateAndNotify(WebRequestState::Completed, m_Connection->get_response_code(), m_Connection->get_response_msg());
		});
		m_Client.set_message_handler([this](TConnectionHandle handle, std::shared_ptr<TMessage> message)
		{
			if (m_State == WebRequestState::Active)
			{
				switch (message->get_opcode())
				{
					case TFrameOpCode::text:
					case TFrameOpCode::binary:
					{
						const std::string& payload = message->get_payload();

						DoPrepareReceiveData();
						if (m_ReceiveStream)
						{
							m_ReceiveStream->Write(payload.data(), payload.size());
						}

						WebRequestEvent event(LockRef(), *m_Response, m_State, payload.data(), payload.size());
						NotifyEvent(WebRequestEvent::EvtDataReceived, event);
						break;
					}
				};
			}
		});

		m_Client.set_tls_init_handler([this](websocketpp::connection_hdl handle) -> std::shared_ptr<TSSLContext>
		{
			if (m_Option_UseSSL != WebRequestOption3::Diabled)
			{
				try
				{
					using namespace boost::asio::ssl;

					std::shared_ptr<TSSLContext> context;
					switch (m_Option_SSLVersion)
					{
						case WebRequestSSLVersion::Default:
						{
							context = websocketpp::lib::make_shared<TSSLContext>(context::sslv23);
							break;
						}
						case WebRequestSSLVersion::SSLv2:
						{
							context = websocketpp::lib::make_shared<TSSLContext>(context::sslv2);
							break;
						}
						case WebRequestSSLVersion::SSLv3:
						{
							context = websocketpp::lib::make_shared<TSSLContext>(context::sslv3);
							break;
						}
						case WebRequestSSLVersion::TLSv1_x:
						{
							context = websocketpp::lib::make_shared<TSSLContext>(context::tls);
							break;
						}
						case WebRequestSSLVersion::TLSv1_0:
						{
							context = websocketpp::lib::make_shared<TSSLContext>(context::tlsv1);
							break;
						}
						case WebRequestSSLVersion::TLSv1_1:
						{
							context = websocketpp::lib::make_shared<TSSLContext>(context::tlsv11);
							break;
						}
						case WebRequestSSLVersion::TLSv1_2:
						{
							context = websocketpp::lib::make_shared<TSSLContext>(context::tlsv12);
							break;
						}
						case WebRequestSSLVersion::TLSv1_3:
						{
							context = websocketpp::lib::make_shared<TSSLContext>(context::tlsv13);
							break;
						}
					};

					if (context)
					{
						// Set peer verify mode
						context->set_verify_mode(m_Option_VerifyPeer == WebRequestOption2::Enabled ? context::verify_peer : context::verify_none);

						// Set max SSL/TLS version to use
						FlagSet<context::options> options = context::default_workarounds|context::single_dh_use;
						switch (m_Option_SSLMaxVersion)
						{
							case WebRequestSSLVersion::Default:
							case WebRequestSSLVersion::TLSv1_x:
							{
								options.Add(context::no_sslv2|context::no_sslv3);
								break;
							}
							case WebRequestSSLVersion::SSLv2:
							{
								options.Add(context::no_sslv3|context::no_tlsv1|context::no_tlsv1_1|context::no_tlsv1_2|context::no_tlsv1_3);
								break;
							}
							case WebRequestSSLVersion::SSLv3:
							{
								options.Add(context::no_tlsv1|context::no_tlsv1_1|context::no_tlsv1_2|context::no_tlsv1_3);
								break;
							}
							case WebRequestSSLVersion::TLSv1_0:
							{
								options.Add(context::no_tlsv1_1|context::no_tlsv1_2|context::no_tlsv1_3);
								break;
							}
							case WebRequestSSLVersion::TLSv1_1:
							{
								options.Add(context::no_tlsv1_2|context::no_tlsv1_3);
								break;
							}
							case WebRequestSSLVersion::TLSv1_2:
							{
								options.Add(context::no_tlsv1_3);
								break;
							}
						};
						context->set_options(options.ToInt());

						return context;
					}
				}
				catch (...)
				{
					wxLogDebug("WSPPWebRequest::DoRegisterHandlers: exception occurred");
				}
			}
			return nullptr;
		});
	}
	void WSPPWebRequest::DoSetRequestHeaders()
	{
		try
		{
			for (const WebRequestHeader& header: m_RequestHeaders)
			{
				m_Connection->append_header(header.GetName().ToUTF8(), header.GetValue().ToUTF8());
			}
		}
		catch (...)
		{
			wxLogDebug("WSPPWebRequest::DoSetRequestHeaders: exception occurred");
		}
	}
	void WSPPWebRequest::DoPrepareSendData()
	{
		try
		{
			switch (m_SendStorage)
			{
				case WebRequestStorage::Memory:
				{
					m_Connection->send(m_SendData, TFrameOpCode::text);

					WebRequestEvent event(LockRef(), m_State, m_SendData.data(), m_SendData.size());
					NotifyEvent(WebRequestEvent::EvtDataSent, event);
					break;
				}
				case WebRequestStorage::Stream:
				case WebRequestStorage::FileSystem:
				{
					// There is no way (I know of) to make WebSocket++ read data from a stream on its own
					if (m_SendStream)
					{
						IO::InputStreamReader reader(*m_SendStream);
						auto buffer = reader.ReadVector<uint8_t>(m_SendStream->GetSize().ToBytes());
						m_Connection->send(buffer.data(), buffer.size(), TFrameOpCode::binary);

						WebRequestEvent event(LockRef(), m_State, buffer.data(), buffer.size());
						NotifyEvent(WebRequestEvent::EvtDataSent, event);
					}
					break;
				}
			};
		}
		catch (...)
		{
			wxLogDebug("WSPPWebRequest::DoPrepareSendData: exception occurred");
		}
	}
	void WSPPWebRequest::DoPrepareReceiveData()
	{
		if (m_ReceiveStorage == WebRequestStorage::Memory)
		{
			m_ReceiveStream = std::make_shared<MemoryOutputStream>();
		}
	}
	void WSPPWebRequest::DoPerformRequest()
	{
		// Prepare the request
		DoResetState();

		if (m_Connection = DoCreateConnection())
		{
			DoSetRequestHeaders();

			// And start the request
			try
			{
				if (m_Client.connect(m_Connection))
				{
					m_Client.run();
				}
				else
				{
					UpdateResponseData();
					ChangeStateAndNotify(WebRequestState::Failed, {}, "Failed to connect");
				}
			}
			catch (...)
			{
				UpdateResponseData();
				if (const auto& status = m_Connection->get_response_msg(); !status.empty())
				{
					ChangeStateAndNotify(WebRequestState::Failed, m_Connection->get_response_code(), status);
				}
				else
				{
					ChangeStateAndNotify(WebRequestState::Failed, m_Connection->get_response_code(), "WSPPWebRequest::DoPerformRequest[m_Client.run]: exception occurred");
				}
			}
		}
		else
		{
			UpdateResponseData();
			ChangeStateAndNotify(WebRequestState::Failed, {}, "Failed to create connection");
		}
	}
	void WSPPWebRequest::DoResetState()
	{
		m_Response.reset();
		m_ResponseHeaders.clear();
		m_Connection = nullptr;
		m_ConnectionHandle = {};

		m_BytesReceived = -1;
		m_BytesExpectedToReceive = -1;
		m_BytesSent = -1;
		m_BytesExpectedToSend = -1;
	}

	WSPPWebRequest::WSPPWebRequest(WSPPWebSession& session, const std::vector<WebRequestHeader>& commonHeaders, const URI& uri)
		:m_Session(session)
	{
		try
		{
			// Initialize the client
			m_Client.clear_access_channels(websocketpp::log::alevel::frame_header);
			m_Client.clear_access_channels(websocketpp::log::alevel::frame_payload);
			m_Client.init_asio();
			DoRegisterHandlers();

			// Make the session object handle events if this object doesn't
			m_EvtHandler.SetNextHandler(&m_Session);

			// Set basic options
			m_URI = uri;
			m_State = WebRequestState::Idle;
			m_RequestHeaders = commonHeaders;
			m_ReceiveStorage = WebRequestStorage::Memory;
		}
		catch (...)
		{
			m_Connection = nullptr;
			m_State = WebRequestState::None;

			wxLogDebug("WSPPWebRequest::WSPPWebRequest: exception occurred");
		}
	}
	WSPPWebRequest::~WSPPWebRequest() noexcept
	{
		m_Client.stop_perpetual();
		if (m_Connection)
		{
			TErrorCode errorCode;
			m_Connection->close(ToInt(WSPPCloseCode::Normal), "WSPPWebRequest::~WSPPWebRequest", errorCode);
			m_Connection = nullptr;
		}
	}

	// IWebRequest: Common
	bool WSPPWebRequest::Start()
	{
		if (m_State == WebRequestState::Idle)
		{
			DoResetState();
			ChangeStateAndNotify(WebRequestState::Started);

			return m_Session.StartRequest(*this);
		}
		return false;
	}
	bool WSPPWebRequest::Pause()
	{
		if (m_State == WebRequestState::Active)
		{
			if (!m_Connection->pause_reading())
			{
				ChangeStateAndNotify(WebRequestState::Paused);
				return true;
			}
		}
		return false;
	}
	bool WSPPWebRequest::Resume()
	{
		if (m_State == WebRequestState::Paused)
		{
			if (!m_Connection->resume_reading())
			{
				NotifyStateChange(WebRequestState::Resumed);
				m_State = WebRequestState::Active;

				return true;
			}
		}
		return false;
	}
	bool WSPPWebRequest::Cancel()
	{
		if (m_State == WebRequestState::Active || m_State == WebRequestState::Paused)
		{
			m_Client.stop_perpetual();

			TErrorCode errorCode;
			m_Connection->close(ToInt(WSPPCloseCode::Normal), "WSPPWebRequest::Cancel", errorCode);
			if (!errorCode)
			{
				return true;
			}
		}
		return false;
	}

	// IWebRequest: Request options
	bool WSPPWebRequest::SetHeader(const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags)
	{
		return WSPPWebSession::SetHeader(m_RequestHeaders, header, flags);
	}
	void WSPPWebRequest::ClearHeaders()
	{
		m_RequestHeaders.clear();
	}

	bool WSPPWebRequest::SetSendStorage(WebRequestStorage storage)
	{
		if (m_State == WebRequestState::Idle || m_State == WebRequestState::Active)
		{
			switch (storage)
			{
				case WebRequestStorage::Memory:
				case WebRequestStorage::Stream:
				case WebRequestStorage::FileSystem:
				{
					m_SendStorage = storage;
					return true;
				}
			};
		}
		return false;
	}
	bool WSPPWebRequest::SetSendSource(std::shared_ptr<IInputStream> stream)
	{
		if (m_State == WebRequestState::Idle || m_State == WebRequestState::Active)
		{
			if (m_SendStream = std::move(stream))
			{
				m_SendData = {};
				m_SendStorage = WebRequestStorage::Stream;

				if (m_Connection)
				{
					DoPrepareSendData();
				}
				return true;
			}
		}
		return false;
	}
	bool WSPPWebRequest::SetSendSource(const String& data)
	{
		if (m_State == WebRequestState::Idle || m_State == WebRequestState::Active)
		{
			m_SendData = data.ToUTF8();
			m_SendStorage = WebRequestStorage::Memory;

			if (m_Connection)
			{
				DoPrepareSendData();
			}
			return true;
		}
		return false;
	}
	bool WSPPWebRequest::SetSendSource(const FSPath& filePath)
	{
		if ((m_State == WebRequestState::Idle || m_State == WebRequestState::Active) && filePath)
		{
			auto& fs = m_Session.GetFileSystem();
			if (WSPPWebRequest::SetSendSource(fs.OpenToRead(filePath)))
			{
				m_SendData = {};
				m_SendStorage = WebRequestStorage::FileSystem;

				if (m_Connection)
				{
					DoPrepareSendData();
				}
				return true;
			}
		}
		return false;
	}

	bool WSPPWebRequest::SetReceiveStorage(WebRequestStorage storage)
	{
		if (m_State == WebRequestState::Idle || m_State == WebRequestState::Active)
		{
			switch (storage)
			{
				case WebRequestStorage::Memory:
				case WebRequestStorage::Stream:
				case WebRequestStorage::FileSystem:
				{
					m_ReceiveStorage = storage;
					return true;
				}
			};
		}
		return false;
	}
	bool WSPPWebRequest::SetReceiveTarget(std::shared_ptr<IOutputStream> stream)
	{
		if (m_State == WebRequestState::Idle || m_State == WebRequestState::Active)
		{
			if (stream)
			{
				m_ReceiveStream = std::move(stream);
				m_ReceiveStorage = WebRequestStorage::Stream;

				return true;
			}
		}
		return false;
	}
	bool WSPPWebRequest::SetReceiveTarget(const FSPath& filePath)
	{
		if ((m_State == WebRequestState::Idle || m_State == WebRequestState::Active) && filePath)
		{
			auto& fs = m_Session.GetFileSystem();
			if (m_ReceiveStream = fs.OpenToWrite(filePath))
			{
				m_ReceiveStorage = WebRequestStorage::FileSystem;
			}
		}
		return false;
	}

	// IWebRequestOptions
	bool WSPPWebRequest::SetURI(const URI& uri)
	{
		m_URI = uri;
		if (m_Connection)
		{
			if (m_Session.m_BaseURI && m_URI.IsReference())
			{
				m_Connection->set_uri(std::make_shared<websocketpp::uri>(m_Session.ResolveURI(m_URI).BuildURI().ToUTF8()));
			}
			else
			{
				m_Connection->set_uri(std::make_shared<websocketpp::uri>(m_URI.BuildURI().ToUTF8()));
			}
		}

		return true;
	}

	bool WSPPWebRequest::SetRequestTimeout(const TimeSpan& timeout)
	{
		return false;
	}
	bool WSPPWebRequest::SetConnectionTimeout(const TimeSpan& timeout)
	{
		const auto value = static_cast<long>(timeout.IsPositive() ? timeout.GetMilliseconds() : 0);
		m_Client.set_open_handshake_timeout(value);
		m_Client.set_close_handshake_timeout(value);

		return true;
	}

	bool WSPPWebRequest::SetKeepAliveIdle(const TimeSpan& interval)
	{
		return false;
	}
	bool WSPPWebRequest::SetKeepAliveInterval(const TimeSpan& interval)
	{
		const auto value = static_cast<long>(interval.IsPositive() ? interval.GetMilliseconds() : 0);

		m_Client.set_pong_timeout(value);
		if (m_Connection)
		{
			m_Connection->set_pong_timeout(value);
		}
		return true;
	}
}
