#pragma once
#include "Common.h"
#include "kxf/Drawing/Color.h"

namespace kxf
{
	class GradientStopItem final
	{
		private:
			Color m_Color;
			float m_Position = 0.0f;

		public:
			GradientStopItem() noexcept = default;
			GradientStopItem(Color color, float position) noexcept
				:m_Color(color), m_Position(position)
			{
			}

		public:
			bool IsNull() const noexcept
			{
				return !m_Color;
			}

			Color GetColor() const noexcept
			{
				return m_Color;
			}
			void SetColor(Color color) noexcept
			{
				m_Color = std::move(color);
			}

			float GetPosition() const noexcept
			{
				return m_Position;
			}
			void SetPosition(float position) noexcept
			{
				m_Position = position;
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			bool operator==(const GradientStopItem& other) const noexcept
			{
				return m_Color == other.m_Color && m_Position == other.m_Position;
			}
			bool operator!=(const GradientStopItem& other) const noexcept
			{
				return m_Color != other.m_Color || m_Position != other.m_Position;
			}
	};
}

namespace kxf
{
	class GradientStops final
	{
		private:
			GradientStopItem m_Start;
			GradientStopItem m_End;
			std::vector<GradientStopItem> m_ExtraStops;

		private:
			void Resort()
			{
				std::stable_sort(m_ExtraStops.begin(), m_ExtraStops.end(), [](const GradientStopItem& left, const GradientStopItem& right)
				{
					return left.GetPosition() < right.GetPosition();
				});
			}

		public:
			GradientStops() noexcept = default;
			GradientStops(Color start, Color end)
				:m_Start(std::move(start), std::numeric_limits<float>::lowest()), m_End(std::move(end), std::numeric_limits<float>::max())
			{
			}

		public:
			bool IsNull() const noexcept
			{
				return !m_Start || !m_End;
			}
			bool HasExtraStops() const noexcept
			{
				return !IsNull() && !m_ExtraStops.empty();
			}

			size_t GetCount() const noexcept
			{
				return IsNull() ? 0 : m_ExtraStops.size() + 2;
			}
			GradientStopItem GetAt(size_t index) const noexcept
			{
				if (index == 0)
				{
					return m_Start;
				}
				else if (const size_t count = GetCount(); index + 1 == count)
				{
					return m_End;
				}
				else if (index < count)
				{
					return m_ExtraStops[index];
				}
				return {};
			}

			Color GetStartColor() const noexcept
			{
				return m_Start.GetColor();
			}
			void SetStartColor(Color color) noexcept
			{
				m_Start.SetColor(std::move(color));
			}

			Color GetEndColor() const noexcept
			{
				return m_End.GetColor();
			}
			void SetEndColor(Color color) noexcept
			{
				m_End.SetColor(std::move(color));
			}

			void Add(GradientStopItem stopItem)
			{
				m_ExtraStops.emplace_back(std::move(stopItem));
				Resort();
			}
			void Add(Color color, float position)
			{
				m_ExtraStops.emplace_back(std::move(color), position);
				Resort();
			}
	};
}
