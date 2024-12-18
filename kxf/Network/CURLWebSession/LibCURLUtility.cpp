#include "kxf-pch.h"
#include "LibCURLUtility.h"
#include "LibCURL.h"
#include "kxf/Utility/ScopeGuard.h"

namespace
{
	std::atomic<bool> g_IsInitialized = false;

	template<bool easy, class THandle, class TOption, class T>
	int DoSetOptionInt(THandle handle, TOption option, T value)
	{
		using ValueT = typename std::remove_cv<T>::type;

		if constexpr(std::is_pointer<ValueT>::value)
		{
			if constexpr(easy)
			{
				return ::curl_easy_setopt(handle, static_cast<CURLoption>(option), (void*)value);
			}
			else
			{
				return ::curl_multi_setopt(handle, static_cast<CURLMoption>(option), (void*)value);
			}
		}
		else if constexpr(sizeof(ValueT) <= sizeof(long))
		{
			if constexpr(easy)
			{
				return ::curl_easy_setopt(handle, static_cast<CURLoption>(option), static_cast<long>(value));
			}
			else
			{
				return ::curl_multi_setopt(handle, static_cast<CURLMoption>(option), static_cast<long>(value));
			}
		}
		else if constexpr(sizeof(ValueT) <= sizeof(curl_off_t))
		{
			if constexpr(easy)
			{
				return ::curl_easy_setopt(handle, static_cast<CURLoption>(option), static_cast<curl_off_t>(value));
			}
			else
			{
				return ::curl_multi_setopt(handle, static_cast<CURLMoption>(option), static_cast<curl_off_t>(value));
			}
		}
		else
		{
			static_assert(sizeof(T*) == 0, "cURL::SetOptionInt: Unknown type");
			if constexpr(easy)
			{
				return CURL_LAST;
			}
			else
			{
				return CURLM_LAST;
			}
		}
	}

	template<class T>
	bool DoDispatchSetOptionInt(kxf::CURL::Private::HandleType type, void* handle, int option, T value)
	{
		using namespace kxf::CURL::Private;

		switch (type)
		{
			case HandleType::Easy:
			{
				int result = DoSetOptionInt<true>(reinterpret_cast<CURL*>(handle), static_cast<CURLoption>(option), value);
				if (result == CURLE_OUT_OF_MEMORY)
				{
					throw std::bad_alloc();
				}
				return result == CURLE_OK;
			}
			case HandleType::Multi:
			{
				int result = DoSetOptionInt<false>(reinterpret_cast<CURLM*>(handle), static_cast<CURLMoption>(option), value);
				if (result == CURLM_OUT_OF_MEMORY)
				{
					throw std::bad_alloc();
				}
				return result == CURLM_OK;
			}
		};
		return false;
	}

	template<class T>
	bool DoGetOption(void* handle, int option, T& value)
	{
		int result = ::curl_easy_getinfo(handle, static_cast<CURLINFO>(option), &value);
		if (result == CURLE_OUT_OF_MEMORY)
		{
			throw std::bad_alloc();
		}
		return result == CURLE_OK;
	}
}

namespace kxf::CURL::Private
{
	RequestHandle::RequestHandle(HandleType type) noexcept
		:m_Type(type)
	{
		switch (m_Type)
		{
			case HandleType::Easy:
			{
				m_Handle = ::curl_easy_init();
				break;
			}
			case HandleType::Multi:
			{
				m_Handle = ::curl_multi_init();
				break;
			}
		};
	}
	RequestHandle::~RequestHandle()
	{
		if (m_Handle)
		{
			switch (m_Type)
			{
				case HandleType::Easy:
				{
					::curl_easy_cleanup(m_Handle);
					break;
				}
				case HandleType::Multi:
				{
					::curl_multi_cleanup(m_Handle);
					break;
				}
			};
			m_Handle = nullptr;
		}
	}

	// Common
	bool RequestHandle::SetOption(int option, const std::string& utf8)
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, utf8.data());
	}
	bool RequestHandle::SetOption(int option, const String& value, size_t* length)
	{
		auto utf8 = value.ToUTF8();
		if (length)
		{
			*length = utf8.length();
		}
		return SetOption(option, utf8);
	}
	bool RequestHandle::SetOption(int option, bool value)
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value ? 1 : 0);
	}
	bool RequestHandle::SetOption(int option, int32_t value)
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	bool RequestHandle::SetOption(int option, uint32_t value)
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	bool RequestHandle::SetOption(int option, int64_t value)
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	bool RequestHandle::SetOption(int option, uint64_t value)
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	bool RequestHandle::SetOption(int option, const void* value)
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}

	// Easy API
	std::string RequestHandle::EscapeString(std::string_view source) const
	{
		if (auto encoded = ::curl_easy_escape(m_Handle, source.data(), source.length()))
		{
			Utility::ScopeGuard atExit = [&]()
			{
				::curl_free(encoded);
			};
			return encoded;
		}
		return {};
	}
	std::string RequestHandle::UnescapeString(std::string_view source) const
	{
		int encodedLength = 0;
		if (auto encoded = ::curl_easy_unescape(m_Handle, source.data(), source.length(), &encodedLength))
		{
			Utility::ScopeGuard atExit = [&]()
			{
				::curl_free(encoded);
			};
			return {encoded, static_cast<size_t>(encodedLength)};
		}
		return {};
	}

	bool RequestHandle::Pause() noexcept
	{
		return m_Type == HandleType::Easy && ::curl_easy_pause(m_Handle, CURLPAUSE_ALL);
	}
	bool RequestHandle::Resume() noexcept
	{
		return m_Type == HandleType::Easy && ::curl_easy_pause(m_Handle, CURLPAUSE_CONT);
	}

	std::optional<std::string_view> RequestHandle::GetOptionUTF8(int option) const
	{
		char* value = nullptr;
		if (DoGetOption(m_Handle, option, value) && value)
		{
			return value;
		}
		return {};
	}
	std::optional<String> RequestHandle::GetOptionString(int option) const
	{
		if (auto utf8 = GetOptionUTF8(option))
		{
			return String::FromUTF8(*utf8);
		}
		return {};
	}
	std::optional<bool> RequestHandle::GetOptionBool(int option) const
	{
		long curlValue = -1;
		if (DoGetOption(m_Handle, option, curlValue))
		{
			return curlValue != 0;
		}
		return {};
	}
	std::optional<int32_t> RequestHandle::GetOptionInt32(int option) const
	{
		long curlValue = -1;
		if (DoGetOption(m_Handle, option, curlValue))
		{
			return static_cast<int32_t>(curlValue);
		}
		return {};
	}
	std::optional<uint32_t> RequestHandle::GetOptionUInt32(int option) const
	{
		long curlValue = -1;
		if (DoGetOption(m_Handle, option, curlValue))
		{
			return static_cast<uint32_t>(curlValue);
		}
		return {};
	}
	std::optional<int64_t> RequestHandle::GetOptionInt64(int option) const
	{
		curl_off_t curlValue = -1;
		if (DoGetOption(m_Handle, option, curlValue))
		{
			return static_cast<int64_t>(curlValue);
		}
		return {};
	}
	std::optional<uint64_t> RequestHandle::GetOptionUInt64(int option) const
	{
		curl_off_t curlValue = -1;
		if (DoGetOption(m_Handle, option, curlValue))
		{
			return static_cast<uint64_t>(curlValue);
		}
		return {};
	}
	std::optional<void*> RequestHandle::GetOptionPtr(int option) const
	{
		void* curlValue = nullptr;
		if (DoGetOption(m_Handle, option, curlValue))
		{
			return curlValue;
		}
		return {};
	}
}

namespace kxf::CURL::Private
{
	bool Initialize() noexcept
	{
		if (!g_IsInitialized)
		{
			g_IsInitialized = ::curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK;
			return g_IsInitialized;
		}
		return true;
	}

	std::string_view EasyErrorCodeToString(int easyErrorCode) noexcept
	{
		if (const char* text = ::curl_easy_strerror(static_cast<CURLcode>(easyErrorCode)))
		{
			return text;
		}
		return {};
	}
}

namespace kxf::CURL::Private
{
	class InitializationModule final: public wxModule
	{
		public:
			bool OnInit() override
			{
				return true;
			}
			void OnExit() override
			{
				if (g_IsInitialized)
				{
					g_IsInitialized = false;
					::curl_global_cleanup();
				}
			}

		private:
			wxDECLARE_DYNAMIC_CLASS(InitializationModule);
	};
}

wxIMPLEMENT_DYNAMIC_CLASS(kxf::CURL::Private::InitializationModule, wxModule);
