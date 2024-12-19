#pragma once
#include "Common.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIImageList.h"
#include <wx/imaglist.h>

// Derive from this class to use 'WithImageList' functions on class that derived from 'wxWithImages'.
// Like this: 'class wxNotebook: public WithImageListWrapper<wxNotebook> { ... };'
namespace kxf::wxWidgets
{
	template<class TBase>
	class WithImageListWrapper: public TBase
	{
		private:
			bool m_IsOwned = false;

		private:
			const TBase& GetThis() const noexcept
			{
				return static_cast<const TBase&>(*this);
			}
			TBase& GetThis() noexcept
			{
				return static_cast<TBase&>(*this);
			}

			static const GDIImageList* ToKxList(const wxImageList& imageList) noexcept
			{
				if (imageList.IsKindOf(wxCLASSINFO(GDIImageList)))
				{
					return static_cast<const GDIImageList*>(&imageList);
				}
				return nullptr;
			}
			static GDIImageList* ToKxList(wxImageList& imageList)
			{
				if (imageList.IsKindOf(wxCLASSINFO(GDIImageList)))
				{
					return static_cast<GDIImageList*>(&imageList);
				}
				return nullptr;
			}

		public:
			bool HasImageList() const noexcept
			{
				return GetImageList() != nullptr;
			}
			bool OwnsImageList() const noexcept
			{
				return m_IsOwned;
			}
			wxIcon GetImage(int index) const = delete;

			wxImageList* GetWxImageList() noexcept
			{
				return GetThis().wxWithImages::GetImageList();
			}
			const wxImageList* GetWxImageList() const noexcept
			{
				return GetThis().wxWithImages::GetImageList();
			}

			GDIImageList* GetImageList() noexcept
			{
				if (wxImageList* imageList = GetWxImageList())
				{
					if (GDIImageList* kxImageList = ToKxList(*imageList))
					{
						return kxImageList;
					}
				}
				return nullptr;
			}
			const GDIImageList* GetImageList() const noexcept
			{
				if (const wxImageList* imageList = GetWxImageList())
				{
					if (const GDIImageList* kxImageList = ToKxList(*imageList))
					{
						return kxImageList;
					}
				}
				return nullptr;
			}

			void SetImageList(const GDIImageList* imageList)
			{
				GetThis().wxWithImages::SetImageList(const_cast<GDIImageList*>(imageList));
				m_IsOwned = false;
			}
			void SetImageList(wxImageList* imageList)
			{
				GetThis().wxWithImages::SetImageList(imageList);
				m_IsOwned = false;
			}

			void AssignImageList(GDIImageList* imageList)
			{
				GetThis().wxWithImages::AssignImageList(imageList);
				m_IsOwned = true;
			}
			void AssignImageList(wxImageList* imageList)
			{
				GetThis().wxWithImages::AssignImageList(imageList);
				m_IsOwned = false;
			}
	};
}
