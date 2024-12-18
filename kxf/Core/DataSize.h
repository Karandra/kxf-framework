#pragma once
#include "Common.h"
#include "String.h"

namespace kxf
{
	enum class DataSizeUnit
	{
		Auto = -1,

		Bytes = 0,
		KiloBytes = 1,
		MegaBytes = 2,
		GigaBytes = 3,
		TeraBytes = 4,
	};
	enum class DataSizeFormat: uint32_t
	{
		None = 0,
		Fractional = 1 << 0,
		WithLabel = 1 << 1,
	};
	kxf_FlagSet_Declare(DataSizeFormat);
}

namespace kxf
{
	class KXF_API DataSize final
	{
		public:
			static constexpr DataSize FromBytes(int64_t bytes) noexcept
			{
				return bytes;
			}
			
			template<class T> requires(std::is_arithmetic_v<T>)
			static constexpr DataSize FromKB(T value) noexcept
			{
				return FromBytes(value * 1024);
			}
			
			template<class T> requires(std::is_arithmetic_v<T>)
			static constexpr DataSize FromMB(T value) noexcept
			{
				return FromKB(value * 1024);
			}
			
			template<class T> requires(std::is_arithmetic_v<T>)
			static constexpr DataSize FromGB(T value) noexcept
			{
				return FromMB(value * 1024);
			}
			
			template<class T> requires(std::is_arithmetic_v<T>)
			static constexpr DataSize FromTB(T value) noexcept
			{
				return FromGB(value * 1024);
			}

			template<class T> requires(std::is_arithmetic_v<T>)
			static constexpr DataSize FromUnit(T value, DataSizeUnit unit) noexcept
			{
				switch (unit)
				{
					case DataSizeUnit::Bytes:
					{
						return FromBytes(value);
					}
					case DataSizeUnit::KiloBytes:
					{
						return FromKB(value);
					}
					case DataSizeUnit::MegaBytes:
					{
						return FromMB(value);
					}
					case DataSizeUnit::GigaBytes:
					{
						return FromGB(value);
					}
					case DataSizeUnit::TeraBytes:
					{
						return FromTB(value);
					}
				};
				return {};
			}

		private:
			int64_t m_Value = -1;

		private:
			template<class T> requires(std::is_integral_v<T>)
			constexpr T GetAs(int64_t value) const noexcept
			{
				return static_cast<T>(value / 1024);
			}

			template<class T> requires(std::is_floating_point_v<T>)
			constexpr T GetAs(int64_t value) const noexcept
			{
				return static_cast<T>(value / 1024.0);
			}

		public:
			constexpr DataSize() noexcept = default;
			constexpr DataSize(int64_t bytes) noexcept
				:m_Value(bytes)
			{
			}

		public:
			// Value testing
			constexpr bool IsNull() const noexcept
			{
				return m_Value == 0;
			}
			constexpr bool IsPositive() const noexcept
			{
				return m_Value > 0;
			}
			constexpr bool IsNegative() const noexcept
			{
				return m_Value < 0;
			}
			constexpr bool IsValid() const noexcept
			{
				return m_Value >= 0;
			}

			// Conversion
			template<class T = int64_t> requires(std::is_arithmetic_v<T>)
			constexpr T ToBytes() const noexcept
			{
				return static_cast<T>(m_Value);
			}
			
			template<class T = int64_t> requires(std::is_arithmetic_v<T>)
			constexpr T ToKB() const noexcept
			{
				return GetAs<T>(m_Value);
			}
			
			template<class T = int64_t> requires(std::is_arithmetic_v<T>)
			constexpr T ToMB() const noexcept
			{
				return GetAs<T>(ToKB());
			}
			
			template<class T = int64_t> requires(std::is_arithmetic_v<T>)
			constexpr T ToGB() const noexcept
			{
				return GetAs<T>(ToMB());
			}
			
			template<class T = int64_t> requires(std::is_arithmetic_v<T>)
			constexpr T ToTB() const noexcept
			{
				return GetAs<T>(ToGB());
			}
			
			template<class T = int64_t> requires(std::is_arithmetic_v<T>)
			constexpr T ToUnit(DataSizeUnit unit) const noexcept
			{
				switch (unit)
				{
					case DataSizeUnit::Bytes:
					{
						return static_cast<T>(ToBytes());
					}
					case DataSizeUnit::KiloBytes:
					{
						return ToKB<T>();
					}
					case DataSizeUnit::MegaBytes:
					{
						return ToMB<T>();
					}
					case DataSizeUnit::GigaBytes:
					{
						return ToGB<T>();
					}
					case DataSizeUnit::TeraBytes:
					{
						return ToTB<T>();
					}
				};
				return -1;
			}

			// Formatting
			String ToString(DataSizeUnit unit = DataSizeUnit::Auto, FlagSet<DataSizeFormat> format = {}, int precision = -1) const;

		public:
			// Comparison
			constexpr auto operator<=>(const DataSize&) const noexcept = default;

			// Arithmetics
			constexpr DataSize operator+(const DataSize& other) const noexcept
			{
				return m_Value + other.m_Value;
			}
			constexpr DataSize operator-(const DataSize& other) const noexcept
			{
				return m_Value - other.m_Value;
			}
			constexpr DataSize operator*(int64_t n) const noexcept
			{
				return m_Value * n;
			}
			constexpr DataSize operator*(double n) const noexcept
			{
				return m_Value * n;
			}
			constexpr DataSize operator/(int64_t n) const noexcept
			{
				return m_Value / n;
			}
			constexpr DataSize operator/(double n) const noexcept
			{
				return m_Value / n;
			}

			constexpr DataSize& operator+=(const DataSize& other) noexcept
			{
				m_Value += other.m_Value;
				return *this;
			}
			constexpr DataSize& operator-=(const DataSize& other) noexcept
			{
				m_Value -= other.m_Value;
				return *this;
			}
			constexpr DataSize& operator*=(int64_t n) noexcept
			{
				m_Value *= n;
				return *this;
			}
			constexpr DataSize& operator*=(double n) noexcept
			{
				m_Value *= n;
				return *this;
			}
			constexpr DataSize& operator/=(int64_t n) noexcept
			{
				m_Value /= n;
				return *this;
			}
			constexpr DataSize& operator/=(double n) noexcept
			{
				m_Value /= n;
				return *this;
			}

			// Validity test
			constexpr explicit operator bool() const noexcept
			{
				return IsValid();
			}
			constexpr bool operator!() const noexcept
			{
				return !IsValid();
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<DataSize> final
	{
		uint64_t Serialize(IOutputStream& stream, const DataSize& value) const
		{
			return Serialization::WriteObject(stream, value.ToBytes());
		}
		uint64_t Deserialize(IInputStream& stream, DataSize& value) const
		{
			int64_t buffer = 0;
			auto read = Serialization::ReadObject(stream, buffer);
			value = DataSize::FromBytes(buffer);

			return read;
		}
	};
}

namespace kxf
{
	inline double GetSizeRatio(DataSize smallerSize, DataSize largerSize) noexcept
	{
		if (smallerSize && largerSize && !largerSize.IsNull())
		{
			return smallerSize.ToBytes<double>() / largerSize.ToBytes<double>();
		}
		return -1;
	}
}
