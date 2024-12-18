#pragma once
#include "../Renderer.h"
#include "TextRenderer.h"
#include "ImageRenderer.h"

namespace kxf::UI::DataView
{
	class KXF_API ImageListValue: public TextValue, public ImageValueBase
	{
		private:
			std::vector<BitmapImage> m_Images;

		public:
			ImageListValue() = default;

			ImageListValue(String text)
				:TextValue(std::move(text))
			{
			}
			ImageListValue(const BitmapImage& image)
			{
				AddImage(image);
			}
			ImageListValue(String text, const BitmapImage& image)
				:TextValue(std::move(text))
			{
				AddImage(image);
			}

		public:
			bool FromAny(Any& value);

			bool HasImages() const
			{
				return !m_Images.empty();
			}
			size_t GetImageCount() const
			{
				return m_Images.size();
			}
			BitmapImage GetBitmap(size_t index) const
			{
				if (index < m_Images.size())
				{
					return m_Images[index];
				}
				return {};
			}
			void AddImage(const BitmapImage& image)
			{
				m_Images.emplace_back(image);
			}
			void ClearImages()
			{
				m_Images.clear();
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API ImageListRendererBase: public Renderer
	{
		private:
			TextValue& m_TextValue;
			ImageValueBase& m_BitmapValueBase;

		protected:
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		protected:
			virtual size_t GetImageCount() const = 0;
			virtual BitmapImage GetImage(size_t index) const = 0;

		public:
			ImageListRendererBase(TextValue& textValue, ImageValueBase& bitmapValueBase, FlagSet<Alignment> alignment = Alignment::Invalid)
				:Renderer(alignment), m_TextValue(textValue), m_BitmapValueBase(bitmapValueBase)
			{
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API ImageListRenderer: public ImageListRendererBase
	{
		private:
			ImageListValue m_Value;

		protected:
			bool SetDisplayValue(Any value) override;

			size_t GetImageCount() const override
			{
				return m_Value.GetImageCount();
			}
			BitmapImage GetImage(size_t index) const override
			{
				return m_Value.GetBitmap(index);
			}

		public:
			ImageListRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:ImageListRendererBase(m_Value, m_Value, alignment)
			{
			}

		public:
			String GetDisplayText(Any value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
