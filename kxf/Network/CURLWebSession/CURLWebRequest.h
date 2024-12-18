#pragma once
#include "Common.h"
#include "../Private/BasicWebRequest.h"
#include "LibCURLUtility.h"
#include "CURLWebResponse.h"
#include "CURLWebAuthChallenge.h"

namespace kxf
{
	class CURLWebSession;
}

namespace kxf
{
	class KXF_API CURLWebRequest final: public RTTI::Implementation<CURLWebRequest, Private::BasicWebRequest, IWebRequestOptions, IWebRequestAuthOptions, IWebRequestSecurityOptions>
	{
		friend class CURLWebResponse;
		friend class CURLWebAuthChallenge;

		private:
			using TCURLOffset = int64_t;

		private:
			static size_t OnReadDataCB(char* data, size_t size, size_t count, void* userData)
			{
				return reinterpret_cast<CURLWebRequest*>(userData)->OnReadData(data, size, count);
			}
			static size_t OnWriteDataCB(char* data, size_t size, size_t count, void* userData)
			{
				return reinterpret_cast<CURLWebRequest*>(userData)->OnWriteData(data, size, count);
			}
			static size_t OnReceiveHeaderCB(char* data, size_t size, size_t count, void* userData)
			{
				return reinterpret_cast<CURLWebRequest*>(userData)->OnReceiveHeader(data, size, count);
			}
			static int OnProgressNotifyCB(void* userData, TCURLOffset bytesExpectedToReceive, TCURLOffset bytesReceived, TCURLOffset bytesExpectedToSend, TCURLOffset bytesSent)
			{
				return reinterpret_cast<CURLWebRequest*>(userData)->OnProgressNotify(bytesReceived, bytesExpectedToReceive, bytesSent, bytesExpectedToSend);
			}

		private:
			// State
			CURLWebSession& m_Session;
			std::atomic<WebRequestState> m_State = WebRequestState::None;
			std::atomic<WebRequestState> m_NextState = WebRequestState::None;

			// Request data
			CURL::Private::RequestHandle m_Handle;
			std::vector<WebRequestHeader> m_RequestHeaders;
			std::vector<WebRequestHeader> m_ResponseHeaders;
			void* m_RequestHeadersSList = nullptr;
			String m_Method;
			URI m_URI;
			WebRequestOption2 m_FollowLocation = WebRequestOption2::Disabled;

			// Send
			std::shared_ptr<IInputStream> m_SendStream;
			std::string m_SendData;
			WebRequestStorage m_SendStorage = WebRequestStorage::None;

			// Receive
			std::shared_ptr<IOutputStream> m_ReceiveStream;
			WebRequestStorage m_ReceiveStorage = WebRequestStorage::None;

			std::optional<CURLWebResponse> m_Response;
			std::optional<CURLWebAuthChallenge> m_AuthChallenge;

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
				WebRequestEvent event(LockRef(), state, std::move(statusCode), std::move(statusText));
				NotifyEvent(WebRequestEvent::EvtStateChanged, event);
			}
			void ChangeStateAndNotify(WebRequestState state, std::optional<int> statusCode = {}, String statusText = {})
			{
				m_State = state;
				NotifyStateChange(state, std::move(statusCode), std::move(statusText));
			}

			void DoFreeRequestHeaders();
			void DoSetRequestHeaders();
			void DoPrepareSendData();
			void DoPrepareReceiveData();
			void DoPerformRequest();
			void DoResetState();

			bool OnCallbackCommon(bool isWrite, size_t& result);
			size_t OnReadData(char* data, size_t size, size_t count);
			size_t OnWriteData(char* data, size_t size, size_t count);
			size_t OnReceiveHeader(char* data, size_t size, size_t count);
			int OnProgressNotify(int64_t bytesReceived, int64_t bytesExpectedToReceive, int64_t bytesSent, int64_t bytesExpectedToSend);
			bool OnSetAuthChallengeCredentials(WebAuthChallengeSource source, UserCredentials credentials);

			// Private::BasicWebRequest
			void PerformRequest() override
			{
				DoPerformRequest();
			}

		public:
			CURLWebRequest(CURLWebSession& session, const std::vector<WebRequestHeader>& commonHeaders, const URI& uri = {});
			~CURLWebRequest() noexcept;

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
				if (m_AuthChallenge)
				{
					return *m_AuthChallenge;
				}
				return NullWebAuthChallenge::Get();
			}
			
			URI GetURI() const override
			{
				return m_URI;
			}
			void* GetNativeHandle() const override
			{
				return *m_Handle;
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
			TransferRate GetSendRate() const override;

			DataSize GetBytesReceived() const override
			{
				return DataSize::FromBytes(m_BytesReceived);
			}
			DataSize GetBytesExpectedToReceive() const override
			{
				return DataSize::FromBytes(m_BytesExpectedToReceive);
			}
			TransferRate GetReceiveRate() const override;

		public:
			// IWebRequestOptions
			bool SetURI(const URI& uri) override;
			bool SetPort(uint16_t port) override;
			bool SetMethod(const String& method) override;
			bool SetDefaultProtocol(const String& protocol) override;
			bool SetAllowedProtocols(FlagSet<WebRequestProtocol> protocols) override;
			bool SetHTTPVersion(WebRequestHTTPVersion option) override;
			bool SetIPVersion(WebRequestIPVersion option) override;

			bool SetServiceName(const String& name) override;
			bool SetAllowRedirection(WebRequestOption2 option) override;
			bool SetRedirectionProtocols(FlagSet<WebRequestProtocol> protocols) override;
			bool SetResumeOffset(DataSize offset) override;

			bool SetRequestTimeout(const TimeSpan& timeout) override;
			bool SetConnectionTimeout(const TimeSpan& timeout) override;

			bool SetMaxSendRate(const TransferRate& rate) override;
			bool SetMaxReceiveRate(const TransferRate& rate) override;

			bool SetKeepAlive(WebRequestOption2 option) override;
			bool SetKeepAliveIdle(const TimeSpan& interval) override;
			bool SetKeepAliveInterval(const TimeSpan& interval) override;

			// IWebRequestAuthOptions
			bool SetAuthMethod(WebRequestAuthMethod method) override;
			bool SetAuthMethods(FlagSet<WebRequestAuthMethod> methods) override;

			bool SetUserName(const String& userName) override;
			bool SetUserPassword(const String& userPassword) override;

			// IWebRequestSecurityOptions
			bool SetUseSSL(WebRequestOption3 option) override;
			bool SetSSLVersion(WebRequestSSLVersion version) override;
			bool SetMaxSSLVersion(WebRequestSSLVersion version) override;

			bool SetVerifyPeer(WebRequestOption2 option) override;
			bool SetVerifyHost(WebRequestOption2 option) override;
			bool SetVerifyStatus(WebRequestOption2 option) override;

		public:
			// CURLWebRequest
			bool IsNull() const noexcept
			{
				return m_Handle.IsNull() || m_State == WebRequestState::None;
			}
	};
}
