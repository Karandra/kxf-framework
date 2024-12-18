#pragma once
#include "Common.h"
#include "IWebRequest.h"
#include "IWebResponse.h"
#include "WebRequestHeader.h"
#include "kxf/EventSystem/Event.h"

namespace kxf
{
	class WebRequestEvent: public BasicEvent
	{
		public:
			kxf_EVENT_MEMBER(WebRequestEvent, StateChanged);
			kxf_EVENT_MEMBER(WebRequestEvent, HeaderReceived);

			kxf_EVENT_MEMBER(WebRequestEvent, DataSent);
			kxf_EVENT_MEMBER(WebRequestEvent, DataReceived);

		private:
			std::shared_ptr<IWebRequest> m_Request;
			IWebResponse* m_Response = nullptr;
			WebRequestState m_State = WebRequestState::None;

			std::optional<int> m_Status;
			String m_StatusText;

			WebRequestHeader m_Header;
			const void* m_Buffer = nullptr;
			size_t m_BufferSize = 0;

		public:
			WebRequestEvent(std::shared_ptr<IWebRequest> request, WebRequestState state)
				:m_Request(std::move(request)), m_State(state)
			{
			}
			WebRequestEvent(std::shared_ptr<IWebRequest> request, WebRequestState state, WebRequestHeader header)
				:m_Request(std::move(request)), m_State(state), m_Header(std::move(header))
			{
			}
			WebRequestEvent(std::shared_ptr<IWebRequest> request, WebRequestState state, const void* buffer, size_t bufferSize)
				:m_Request(std::move(request)), m_State(state), m_Buffer(buffer), m_BufferSize(bufferSize)
			{
			}
			WebRequestEvent(std::shared_ptr<IWebRequest> request, WebRequestState state, std::optional<int> status = {}, String statusText = {})
				:m_Request(std::move(request)), m_State(state), m_Status(status), m_StatusText(std::move(statusText))
			{
			}

			WebRequestEvent(std::shared_ptr<IWebRequest> request, IWebResponse& response, WebRequestState state, const void* buffer, size_t bufferSize)
				:m_Request(std::move(request)), m_Response(&response), m_State(state), m_Buffer(buffer), m_BufferSize(bufferSize)
			{
			}
			WebRequestEvent(std::shared_ptr<IWebRequest> request, IWebResponse& response, WebRequestState state, std::optional<int> status = {}, String statusText = {})
				:m_Request(std::move(request)), m_Response(&response), m_State(state), m_Status(status), m_StatusText(std::move(statusText))
			{
			}
			WebRequestEvent(std::shared_ptr<IWebRequest> request, IWebResponse& response, std::optional<int> status = {}, String statusText = {})
				:m_Request(std::move(request)), m_Response(&response), m_State(WebRequestState::Completed), m_Status(status), m_StatusText(std::move(statusText))
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WebRequestEvent>(std::move(*this));
			}

			// WebRequestEvent
			IWebRequest& GetRequest() const
			{
				return *m_Request;
			}
			IWebResponse& GetResponse() const
			{
				return m_Response ? *m_Response : NullWebResponse::Get();
			}
			WebRequestState GetState() const
			{
				return m_State;
			}

			const WebRequestHeader& GetHeader() const&
			{
				return m_Header;
			}
			WebRequestHeader GetHeader() &&
			{
				return std::move(m_Header);
			}

			std::optional<int> GetStatusCode() const
			{
				return m_Status;
			}
			const String& GetStatusText() const&
			{
				return m_StatusText;
			}
			String GetStatusText() &&
			{
				return std::move(m_StatusText);
			}

			const void* GetBuffer() const
			{
				return m_Buffer;
			}
			size_t GetBufferSize() const
			{
				return m_BufferSize;
			}
	};
}
