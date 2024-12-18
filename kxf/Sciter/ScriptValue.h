#pragma once
#include "Common.h"

namespace kxf::Sciter
{
	enum class ScriptValueType: uint32_t
	{
		None = 0,
		Null,
		Bool,
		Int,
		Float,
		String,
		DateTime,
		Length,
		Array,
		Map,
		Function,
		Bytes,
		Object,
		Resource,
		Range,
		Duration,
		Angle,
		Color
	};
	struct ScriptNativeValue
	{
		ScriptValueType Type = ScriptValueType::None;
		uint32_t Value32 = 0;
		uint64_t Value64 = 0;
	};
}

namespace kxf::Sciter
{
	class KXF_API ScriptValue final
	{
		public:
			using TFunction = std::function<ScriptValue(const ScriptValue* arguments, size_t count)>;
			enum class StringType
			{
				None,
				Symbol,
				Secure,
				Error
			};

		private:
			ScriptNativeValue m_Value;

		private:
			void Init();
			void Clear();
			void Copy(const ScriptNativeValue& other);
			bool IsEqual(const ScriptNativeValue& other) const;
			void AssingString(StringView data, StringType type);

		public:
			ScriptValue()
			{
				Init();
			}
			ScriptValue(const ScriptValue& other)
			{
				Init();
				Copy(other.m_Value);
			}
			ScriptValue(const ScriptNativeValue& other)
			{
				Init();
				Copy(other);
			}

			template<class T>
			ScriptValue(T&& value)
			{
				Init();
				*this = std::forward<T>(value);
			}

			~ScriptValue()
			{
				Clear();
			}

		public:
			bool IsNull() const;
			bool IsUndefined() const;
			ScriptValueType GetType() const;
			
			const ScriptNativeValue& GetNativeValue() const
			{
				return m_Value;
			}
			ScriptNativeValue& GetNativeValue()
			{
				return m_Value;
			}

			// Retrieving
			String GetString() const;
			std::optional<int> GetInt() const;
			std::optional<int64_t> GetInt64() const;
			std::optional<double> GetFloat() const;
			DateTime GetDateTime() const;
			TimeSpan GetDuration() const;
			Color GetColor() const;
			Angle GetAngle() const;
			const void* GetBytes(size_t& size) const;

			// Assignment
			ScriptValue& operator=(int value);
			ScriptValue& operator=(int64_t value);
			ScriptValue& operator=(double value);
			ScriptValue& operator=(bool value);
			ScriptValue& operator=(const String& value)
			{
				SetString(value);
				return *this;
			}
			ScriptValue& operator=(std::string_view value)
			{
				SetString(value);
				return *this;
			}
			ScriptValue& operator=(std::wstring_view value)
			{
				SetString(value);
				return *this;
			}
			ScriptValue& operator=(const char* value)
			{
				SetString(value);
				return *this;
			}
			ScriptValue& operator=(const wchar_t* value)
			{
				SetString(value);
				return *this;
			}
			ScriptValue& operator=(const DateTime& value);
			ScriptValue& operator=(const TimeSpan& value);
			ScriptValue& operator=(const Color& value);
			ScriptValue& SetAngle(Angle angle);
			ScriptValue& SetBytes(const void* data, size_t size);
			ScriptValue& SetString(const String& value, StringType type = StringType::None)
			{
				AssingString(StringView(value.wc_str(), value.length()), type);
				return *this;
			}

			ScriptValue& operator=(const ScriptValue& other)
			{
				Copy(other.m_Value);
				return *this;
			}
			ScriptValue& operator=(const ScriptNativeValue& other)
			{
				Copy(other);
				return *this;
			}

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}

			bool operator==(const ScriptValue& other) const
			{
				return IsEqual(other.m_Value);
			}
			bool operator!=(const ScriptValue& other) const
			{
				return !(*this == other);
			}
	};
}
