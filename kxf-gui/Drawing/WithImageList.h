#pragma once
#include "Common.h"
#include "GDIRenderer/GDIImageList.h"

namespace kxf
{
	class KXF_API WithImageList
	{
		private:
			GDIImageList* m_ImageList = nullptr;
			bool m_IsOwned = false;

		private:
			void DeleteIfNeeded() noexcept
			{
				if (m_IsOwned)
				{
					delete m_ImageList;
					m_IsOwned = false;
				}
				m_ImageList = nullptr;
			}

		public:
			WithImageList() noexcept = default;
			virtual ~WithImageList() noexcept
			{
				DeleteIfNeeded();
			}

		public:
			bool HasImageList() const noexcept
			{
				return m_ImageList != nullptr;
			}
			bool OwnsImageList() const noexcept
			{
				return m_IsOwned;
			}

			GDIImageList* GetImageList() noexcept
			{
				return m_ImageList;
			}
			const GDIImageList* GetImageList() const noexcept
			{
				return m_ImageList;
			}

			virtual void SetImageList(const GDIImageList* imageList)
			{
				DeleteIfNeeded();
				m_IsOwned = false;
				m_ImageList = const_cast<GDIImageList*>(imageList);
			}
			virtual void AssignImageList(GDIImageList* imageList)
			{
				DeleteIfNeeded();
				m_IsOwned = true;
				m_ImageList = imageList;
			}
	};
}
