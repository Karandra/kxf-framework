#pragma once
#include "../Renderer.h"

namespace kxf::UI::DataView
{
	class KXF_API ImageValueBase
	{
		protected:
			Size m_DefaultSize = Size::UnspecifiedSize();
			int m_ImageSpacing = 0;
			bool m_DrawInvalid = false;

		public:
			ImageValueBase() = default;

		public:
			int GetImageSpacing() const
			{
				return m_ImageSpacing;
			}
			void SetImageSpacing(int value)
			{
				m_ImageSpacing = value;
			}

			bool ShouldDrawInvalidImages() const
			{
				return m_DrawInvalid;
			}
			void DrawInvalidImages(bool value = true)
			{
				m_DrawInvalid = value;
			}

			Size GetDefaultImagesSize() const
			{
				return m_DefaultSize;
			}
			void SetDefaultImagesSize(const Size& value)
			{
				m_DefaultSize = value;
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API ImageValue: public ImageValueBase
	{
		protected:
			BitmapImage m_Image;

		public:
			ImageValue(const BitmapImage& image = {})
				:m_Image(image)
			{
			}

		public:
			bool FromAny(Any& value);

			bool HasImage() const
			{
				return !m_Image.IsNull();
			}
			BitmapImage& GetImage()
			{
				return m_Image;
			}
			const BitmapImage& GetImage() const
			{
				return m_Image;
			}
			void SetImage(const BitmapImage& image)
			{
				m_Image = image;
			}
			void ClearImage()
			{
				m_Image = {};
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API ImageRenderer: public Renderer
	{
		private:
			ImageValue m_Value;

		protected:
			bool SetDisplayValue(Any value) override;
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			ImageRenderer(FlagSet<Alignment> alignment = Alignment::CenterHorizontal|Alignment::CenterVertical)
				:Renderer(alignment)
			{
			}

		public:
			String GetDisplayText(Any value) const override
			{
				return {};
			}
	};
}
