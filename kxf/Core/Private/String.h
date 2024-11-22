#pragma once
#include "../Common.h"
#include <string>
#include <string_view>

namespace kxf::Private
{
	template<class TChar>
	constexpr size_t CalcStringLength(const TChar* data, size_t length) noexcept
	{
		if (data)
		{
			if (length == std::basic_string_view<TChar>::npos)
			{
				return std::char_traits<TChar>::length(data);
			}
			return length;
		}
		return 0;
	}
}

namespace kxf::Private
{
	template<class TChar_>
	class BasicStrViewAdapter
	{
		public:
			using TChar = typename TChar_;
			using TString = std::basic_string<TChar_>;
			using TStringView = std::basic_string_view<TChar_>;

			static constexpr size_t npos = TStringView::npos;

		protected:
			TStringView m_View;

		public:
			BasicStrViewAdapter() noexcept = default;
			BasicStrViewAdapter(const TChar_* ptr, size_t length = npos) noexcept
				:m_View(ptr, CalcStringLength(ptr, length))
			{
			}
			BasicStrViewAdapter(TStringView str) noexcept
				:m_View(std::move(str))
			{
			}
			BasicStrViewAdapter(const TString& str) noexcept
				:m_View(str.data(), str.length())
			{
			}

		public:
			TStringView GetView() const noexcept
			{
				return m_View;
			}
	};

	template<class TChar_>
	class BasicConvertedStrBuffer
	{
		public:
			using TChar = typename TChar_;
			using TString = std::basic_string<TChar_>;
			using TStringView = std::basic_string_view<TChar_>;

		private:
			TString m_Data;

		public:
			BasicConvertedStrBuffer() noexcept = default;
			BasicConvertedStrBuffer(TString data) noexcept
				:m_Data(std::move(data))
			{
			}

		public:
			TString str() const&
			{
				return m_Data;
			}
			TString str()&& noexcept
			{
				return std::move(m_Data);
			}
			TStringView view() const noexcept
			{
				return m_Data;
			}

			uintptr_t intptr() const noexcept
			{
				return reinterpret_cast<uintptr_t>(m_Data.data());
			}
			TChar* data() noexcept
			{
				return m_Data.data();
			}
			const TChar* data() const noexcept
			{
				return m_Data.data();
			}
			size_t length() const noexcept
			{
				return m_Data.length();
			}

		public:
			operator TString() const&
			{
				return m_Data;
			}
			operator TString()&& noexcept
			{
				return std::move(m_Data);
			}
			operator TStringView() const noexcept
			{
				return m_Data;
			}

			operator TChar* () noexcept
			{
				return m_Data.data();
			}
			operator const TChar*() const noexcept
			{
				return m_Data.data();
			}
	};

	template<class TChar_>
	class BasicUnownedStrBuffer
	{
		public:
			using TChar = typename TChar_;
			using TString = std::basic_string<TChar_>;
			using TStringView = std::basic_string_view<TChar_>;

		private:
			TStringView m_View;

		public:
			BasicUnownedStrBuffer() noexcept = default;
			BasicUnownedStrBuffer(TStringView view) noexcept
				:m_View(std::move(view))
			{
			}

		public:
			TString str() const
			{
				return m_View;
			}
			TStringView view() const noexcept
			{
				return m_View;
			}

			uintptr_t unsafe_intptr() const noexcept
			{
				return reinterpret_cast<uintptr_t>(m_View.data());
			}
			TChar* unsafe_data() const noexcept
			{
				return const_cast<TChar*>(m_View.data());
			}
			const TChar* data() const noexcept
			{
				return m_View.data();
			}
			size_t length() const noexcept
			{
				return m_View.length();
			}

		public:
			operator TString() const noexcept
			{
				return m_View;
			}
			operator TStringView() const noexcept
			{
				return m_View;
			}
			operator const TChar*() const noexcept
			{
				return m_View.data();
			}
	};
}

namespace kxf
{
	class CStrViewAdapter final: public Private::BasicStrViewAdapter<char>
	{
		public:
			using BasicStrViewAdapter::BasicStrViewAdapter;

			CStrViewAdapter(const char8_t* ptr, size_t length = npos) noexcept
				:BasicStrViewAdapter(reinterpret_cast<const char*>(ptr), Private::CalcStringLength(ptr, length))
			{
				static_assert(sizeof(char) == sizeof(char8_t), "char must be the same size as char8_t");
			}
	};

	class WStrViewAdapter final: public Private::BasicStrViewAdapter<wchar_t>
	{
		public:
			using BasicStrViewAdapter::BasicStrViewAdapter;

			WStrViewAdapter(const char16_t* ptr, size_t length = npos) noexcept
				:BasicStrViewAdapter(reinterpret_cast<const wchar_t*>(ptr), Private::CalcStringLength(ptr, length))
			{
				static_assert(sizeof(wchar_t) == sizeof(char16_t), "wchar_t must be the same size as char16_t");
			}
	};

	using ConvertedCStrBuffer = Private::BasicConvertedStrBuffer<char>;
	using ConvertedWStrBuffer = Private::BasicConvertedStrBuffer<wchar_t>;

	using UnownedCStrBuffer = Private::BasicUnownedStrBuffer<char>;
	using UnownedWStrBuffer = Private::BasicUnownedStrBuffer<wchar_t>;
}
