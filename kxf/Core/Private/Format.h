#pragma once
#include "../Common.h"

namespace kxf
{
	namespace Private
	{
		template<class T_>
		struct FormatAdapter final: std::false_type
		{
			using T = typename T_;

			static T& Convert(T& arg) noexcept
			{
				return arg;
			}
			static const T& Convert(const T& arg) noexcept
			{
				return arg;
			}
		};

		template<>
		struct FormatAdapter<char*> final: std::true_type
		{
			using T = String;

			static T Convert(const char* arg)
			{
				return StringViewOf(arg);
			}
		};

		template<>
		struct FormatAdapter<const char*> final: std::true_type
		{
			using T = String;

			static T Convert(const char* arg)
			{
				return StringViewOf(arg);
			}
		};

		template<>
		struct FormatAdapter<wchar_t*> final: std::true_type
		{
			using T = std::basic_string_view<wchar_t>;

			static T Convert(const wchar_t* arg)
			{
				return StringViewOf(arg);
			}
		};

		template<>
		struct FormatAdapter<const wchar_t*> final: std::true_type
		{
			using T = std::basic_string_view<wchar_t>;

			static T Convert(const wchar_t* arg)
			{
				return StringViewOf(arg);
			}
		};

		template<size_t N>
		struct FormatAdapter<char[N]> final: std::true_type
		{
			using T = String;

			static T Convert(const char (&arg)[N])
			{
				return StringViewOf(arg);
			}
		};

		template<size_t N>
		struct FormatAdapter<wchar_t[N]> final: std::true_type
		{
			using T = std::basic_string_view<wchar_t>;

			static T Convert(const wchar_t (&arg)[N])
			{
				return StringViewOf(arg);
			}
		};

		template<>
		struct FormatAdapter<std::string> final: std::true_type
		{
			using T = String;

			static T Convert(const std::string& arg)
			{
				return StringViewOf(arg);
			}
		};

		template<>
		struct FormatAdapter<std::string_view> final: std::true_type
		{
			using T = String;

			static T Convert(std::string_view arg)
			{
				return arg;
			}
		};

		template<class T_>
		using FormatAdapter_ = FormatAdapter<std::remove_const_t<std::remove_reference_t<T_>>>;

		template<class... Args>
		class FormatStore final
		{
			private:
				std::tuple<typename FormatAdapter_<Args>::T...> m_Args;

			public:
				template<class... Args>
				FormatStore(Args&&... arg)
					:m_Args{(FormatAdapter_<Args>::value ? FormatAdapter_<Args>::Convert(std::forward<Args>(arg)) : std::forward<Args>(arg))...}
				{
				}

			public:
				auto MakeFormatArgs() const
				{
					return std::apply([&]<class... Args>(Args&... arg)
					{
						return std::make_wformat_args(arg...);
					}, m_Args);
				};
				size_t CalcFormatSize(std::wstring_view format) const
				{
					return std::apply([&]<class... Args>(Args&&... arg)
					{
						return std::formatted_size(format, std::forward<Args>(arg)...);
					}, m_Args);
				};
		};

		template<class... Args>
		decltype(auto) CreateFormatStore(Args&&... arg)
		{
			return FormatStore<Args...>(std::forward<Args>(arg)...);
		}
	}

	namespace Private
	{
		void LogFormatterException(const std::format_error& e);

		template<class... Args>
		String DoFormat(std::wstring_view format, Args&&... arg)
		{
			try
			{
				auto store = CreateFormatStore(std::forward<Args>(arg)...);
				return std::vformat(format, store.MakeFormatArgs());
			}
			catch (const std::format_error& e)
			{
				LogFormatterException(e);
			}
			return String(format);
		}

		template<class... Args>
		String DoFormat(std::string_view format, Args&&... arg)
		{
			auto converted = String::FromUnknownEncoding(format);
			return DoFormat(converted.view(), std::forward<Args>(arg)...);
		}

		template<class OutputIt, class... Args>
		OutputIt DoFormatTo(OutputIt outputIt, std::wstring_view format, Args&&... arg)
		{
			try
			{
				auto store = CreateFormatStore(std::forward<Args>(arg)...);
				return std::vformat_to(outputIt, format, store.MakeFormatArgs());
			}
			catch (const std::format_error& e)
			{
				LogFormatterException(e);
			}
			return outputIt;
		}

		template<class OutputIt, class... Args>
		OutputIt DoFormatTo(OutputIt outputIt, std::string_view format, Args&&... arg)
		{
			auto converted = String::FromUnknownEncoding(format);
			return DoFormatTo(outputIt, converted.view(), std::forward<Args>(arg)...);
		}

		template<class... Args>
		size_t DoFormattedSize(std::wstring_view format, Args&&... arg)
		{
			try
			{
				auto store = CreateFormatStore(std::forward<Args>(arg)...);
				return store.CalcFormatSize(format);
			}
			catch (const std::format_error& e)
			{
				LogFormatterException(e);
			}
			return 0;
		}

		template<class... Args>
		size_t DoFormattedSize(std::string_view format, Args&&... arg)
		{
			auto converted = String::FromUnknownEncoding(format);
			return DoFormattedSize(converted.view(), std::forward<Args>(arg)...);
		}
	}

	template<class TFormat, class... Args>
	String Format(const TFormat& format, Args&&... arg)
	{
		return Private::DoFormat(StringViewOf(format), std::forward<Args>(arg)...);
	}

	template<class OutputIt, class TFormat, class... Args>
	OutputIt FormatTo(OutputIt outputIt, const TFormat& format, Args&&... arg)
	{
		return Private::DoFormatTo(outputIt, StringViewOf(format), std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	size_t FormattedSize(const TFormat& format, Args&&... arg)
	{
		return Private::DoFormattedSize(StringViewOf(format), std::forward<Args>(arg)...);
	}
}

namespace kxf
{
	template<class OutputIt, class... Args>
	OutputIt String::FormatTo(OutputIt outputIt, std::string_view format, Args&&... arg)
	{
		return Private::DoFormatTo(outputIt, format, std::forward<Args>(arg)...);
	}

	template<class OutputIt, class... Args>
	OutputIt String::FormatTo(OutputIt outputIt, std::wstring_view format, Args&&... arg)
	{
		return Private::DoFormatTo(outputIt, format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	String& String::Format(const TFormat& format, Args&&... arg)
	{
		Private::DoFormatTo(std::back_inserter(m_String), StringViewOf(format), std::forward<Args>(arg)...);
		return *this;
	}

	template<class TFormat, class... Args>
	String& String::FormatAt(size_t position, const TFormat& format, Args&&... arg)
	{
		position = std::clamp(position, 0, m_String.size());
		Private::DoFormatTo(std::inserter(m_String, m_String.begin() + position), StringViewOf(format), std::forward<Args>(arg)...);

		return *this;
	}
}
