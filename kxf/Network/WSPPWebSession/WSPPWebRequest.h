#pragma once
#include "Common.h"
#include "WSPP.h"
#include "WSPPWebResponse.h"
#include "../Private/BasicWebRequest.h"

namespace kxf
{
	class WSPPWebSession;
}

namespace kxf
{
	class KXF_API WSPPWebRequest final: public RTTI::Implementation<WSPPWebRequest, Private::BasicWebRequest, IWebRequestOptions, IWebRequestSecurityOptions>, public Private::WSPPTypes
	{
		friend class WSPPWebResponse;

		private:
			// State
			WSPPWebSession& m_Session;
			TClient m_Client;
			TConnectionHandle m_ConnectionHandle;
			std::shared_ptr<TConnection> m_Connection;

			std::atomic<WebRequestState> m_State = WebRequestState::None;
			std::atomic<WebRequestState> m_NextState = WebRequestState::None;

			// Request data
			URI m_URI;
			std::optional<WSPPWebResponse> m_Response;
			std::vector<WebRequestHeader> m_RequestHeaders;
			std::vector<WebRequestHeader> m_ResponseHeaders;

			WebRequestOption3 m_Option_UseSSL = WebRequestOption3::Enabled;
			WebRequestOption2 m_Option_VerifyPeer = WebRequestOption2::Enabled;
			WebRequestSSLVersion m_Option_SSLVersion = WebRequestSSLVersion::Default;
			WebRequestSSLVersion m_Option_SSLMaxVersion = WebRequestSSLVersion::Default;

			// Send
			std::shared_ptr<IInputStream> m_SendStream;
			std::string m_SendData;
			WebRequestStorage m_SendStorage = WebRequestStorage::None;

			// Receive
			std::shared_ptr<IOutputStream> m_ReceiveStream;
			WebRequestStorage m_ReceiveStorage = WebRequestStorage::None;

			// Progress state
			std::atomic<int64_t> m_BytesReceived = -1;
			std::atomic<int64_t> m_BytesExpectedToReceive = -1;
			std::atomic<int64_t> m_BytesSent = -1;
			std::atomic<int64_t> m_BytesExpectedToSend = -1;

		private:
			void NotifyEvent(EventTag<WebRequestEvent> eventID, WebRequestEvent& event)
			{
				m_EvtHandler.ProcessEvent(event, eventID, ProcessEventFlag::HandleExceptions);
			}
			void NotifyStateChange(WebRequestState state, std::optional<int> statusCode = {}, String statusText = {})
			{
				if (m_Response)
				{
					WebRequestEvent event(LockRef(), *m_Response, state, std::move(statusCode), std::move(statusText));
					NotifyEvent(WebRequestEvent::EvtStateChanged, event);
				}
				else
				{
					WebRequestEvent event(LockRef(), state, std::move(statusCode), std::move(statusText));
					NotifyEvent(WebRequestEvent::EvtStateChanged, event);
				}
			}
			void ChangeStateAndNotify(WebRequestState state, std::optional<int> statusCode = {}, String statusText = {})
			{
				m_State = state;
				NotifyStateChange(state, std::move(statusCode), std::move(statusText));
			}
			void UpdateResponseData()
			{
				if (m_Response && m_Connection)
				{
					m_Response->m_StatusCode = m_Connection->get_response_code();
					m_Response->m_StatusText = m_Connection->get_response_msg();
				}
			}

			std::shared_ptr<TConnection> DoCreateConnection();
			void DoRegisterHandlers();
			void DoSetRequestHeaders();
			void DoPrepareSendData();
			void DoPrepareReceiveData();
			void DoPerformRequest();
			void DoResetState();

			// Private::BasicWebRequest
			void PerformRequest() override
			{
				DoPerformRequest();
			}

		public:
			WSPPWebRequest(WSPPWebSession& session, const std::vector<WebRequestHeader>& commonHeaders, const URI& uri = {});
			~WSPPWebRequest() noexcept;

		public:
			// IWebRequest: Common
			bool Start() override;
			bool Pause() override;
			bool Resume() override;
			bool Cancel() override;

			IWebResponse& GetResponse() override
			{
				if (m_Response)
				{
					return *m_Response;
				}
				return NullWebResponse::Get();
			}
			IWebAuthChallenge& GetAuthChallenge() override
			{
				return NullWebAuthChallenge::Get();
			}
			
			URI GetURI() const override
			{
				return m_URI;
			}
			void* GetNativeHandle() const override
			{
				return nullptr;
			}

			// IWebRequest: Request options
			bool SetHeader(const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags) override;
			void ClearHeaders() override;

			bool SetSendStorage(WebRequestStorage storage) override;
			bool SetSendSource(std::shared_ptr<IInputStream> stream) override;
			bool SetSendSource(const FSPath& filePath) override;
			bool SetSendSource(const String& data) override;

			bool SetReceiveStorage(WebRequestStorage storage) override;
			bool SetReceiveTarget(std::shared_ptr<IOutputStream> stream) override;
			bool SetReceiveTarget(const FSPath& filePath) override;

			// IWebRequest: Progress
			WebRequestState GetState() const override
			{
				return m_State;
			}

			DataSize GetBytesSent() const override
			{
				return DataSize::FromBytes(m_BytesSent);
			}
			DataSize GetBytesExpectedToSend() const override
			{
				return DataSize::FromBytes(m_BytesExpectedToSend);
			}
			TransferRate GetSendRate() const override
			{
				return {};
			}

			DataSize GetBytesReceived() const override
			{
				return DataSize::FromBytes(m_BytesReceived);
			}
			DataSize GetBytesExpectedToReceive() const override
			{
				return DataSize::FromBytes(m_BytesExpectedToReceive);
			}
			TransferRate GetReceiveRate() const override
			{
				return {};
			}

		public:
			// IWebRequestOptions
			bool SetURI(const URI& uri) override;
			bool SetPort(uint16_t port) override
			{
				return false;
			}
			bool SetMethod(const String& method) override
			{
				return false;
			}
			bool SetDefaultProtocol(const String& protocol) override
			{
				return false;
			}
			bool SetAllowedProtocols(FlagSet<WebRequestProtocol> protocols) override
			{
				return false;
			}
			bool SetHTTPVersion(WebRequestHTTPVersion option) override
			{
				return false;
			}
			bool SetIPVersion(WebRequestIPVersion option) override
			{
				return false;
			}

			bool SetServiceName(const String& name) override
			{
				return false;
			}
			bool SetAllowRedirection(WebRequestOption2 option) override
			{
				return false;
			}
			bool SetRedirectionProtocols(FlagSet<WebRequestProtocol> protocols) override
			{
				return false;
			}
			bool SetResumeOffset(DataSize offset) override
			{
				return false;
			}

			bool SetRequestTimeout(const TimeSpan& timeout) override;
			bool SetConnectionTimeout(const TimeSpan& timeout) override;

			bool SetMaxSendRate(const TransferRate& rate) override
			{
				return false;
			}
			bool SetMaxReceiveRate(const TransferRate& rate) override
			{
				return false;
			}

			bool SetKeepAlive(WebRequestOption2 option) override
			{
				return false;
			}
			bool SetKeepAliveIdle(const TimeSpan& interval) override;
			bool SetKeepAliveInterval(const TimeSpan& interval) override;

			// IWebRequestSecurityOptions
			bool SetUseSSL(WebRequestOption3 option) override
			{
				m_Option_UseSSL = option;
				return true;
			}
			bool SetSSLVersion(WebRequestSSLVersion version) override
			{
				m_Option_SSLVersion = version;
				return true;
			}
			bool SetMaxSSLVersion(WebRequestSSLVersion version) override
			{
				m_Option_SSLVersion = version;
				return true;
			}

			bool SetVerifyPeer(WebRequestOption2 option) override
			{
				m_Option_VerifyPeer = option;
				return true;
			}
			bool SetVerifyHost(WebRequestOption2 option) override
			{
				return false;
			}
			bool SetVerifyStatus(WebRequestOption2 option) override
			{
				return false;
			}

		public:
			// WSPPWebRequest
			bool IsNull() const noexcept
			{
				return m_State == WebRequestState::None;
			}
	};
}
