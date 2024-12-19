#pragma once
#include "../Renderer.h"
#include "ImageListRenderer.h"
#include "kxf-gui/Drawing/WithImageList.h"

namespace kxf::UI::DataView
{
	class KXF_API GDIImageListValue: public TextValue, public ImageValueBase, public WithImageList
	{
		public:
			GDIImageListValue() = default;
			GDIImageListValue(const String& text)
				:TextValue(text)
			{
			}
			GDIImageListValue(const GDIImageList& imageList)
			{
				WithImageList::SetImageList(&imageList);
			}
			GDIImageListValue(std::unique_ptr<GDIImageList> imageList)
			{
				WithImageList::AssignImageList(imageList.release());
			}

		public:
			bool FromAny(Any& value);

			bool HasImages() const
			{
				return GetImageCount() != 0;
			}
			size_t GetImageCount() const
			{
				if (const GDIImageList* imageList = WithImageList::GetImageList())
				{
					return imageList->GetImageCount();
				}
				return 0;
			}
			GDIBitmap GetImage(size_t index) const
			{
				const GDIImageList* imageList = WithImageList::GetImageList();
				if (imageList && index < imageList->GetImageCount())
				{
					return imageList->GetBitmap(index);
				}
				return {};
			}
			void AddImage(const GDIBitmap& bitmap)
			{
				if (GDIImageList* imageList = WithImageList::GetImageList())
				{
					imageList->Add(bitmap);
				}
			}
			void ClearImages()
			{
				if (GDIImageList* imageList = WithImageList::GetImageList())
				{
					imageList->RemoveAll();
				}
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API GDIImageListRenderer: public ImageListRendererBase
	{
		private:
			GDIImageListValue m_Value;

		protected:
			bool SetDisplayValue(Any value) override;

			size_t GetImageCount() const override
			{
				return m_Value.GetImageCount();
			}
			BitmapImage GetImage(size_t index) const override
			{
				return m_Value.GetImage(index).AsWXBitmap();
			}

		public:
			GDIImageListRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
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
