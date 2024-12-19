#pragma once
#include "Common.h"

#include <Windows.h>
#include <wx/gdicmn.h>
#include <wx/msw/gdiimage.h>

namespace kxf::Drawing::Private
{
	template<std::derived_from<wxGDIImage> T>
	void* DetachGDIImageHandle(T& gdiImage)
	{
		if (gdiImage.IsOk())
		{
			void* handle = nullptr;
			std::swap(handle, gdiImage.GetGDIImageData()->m_handle);
			gdiImage = T{};

			return handle;
		}
		return nullptr;
	}

	template<class TFunc>
	bool AttachIconHandle(wxGDIImage& gdiImage, void* handle, TFunc&& unshareFunc)
	{
		if (handle)
		{
			ICONINFOEXW iconInfo = {};
			if (::GetIconInfoExW(reinterpret_cast<HICON>(handle), &iconInfo))
			{
				auto GetBitmapInfo = [](void* handle, Size& size, ColorDepth& depth)
				{
					BITMAP bitmap = {};
					if (::GetObject(handle, sizeof(bitmap), &bitmap) != 0)
					{
						size = {bitmap.bmWidth, bitmap.bmHeight};
						depth = bitmap.bmBitsPixel;

						return true;
					}
					return false;
				};

				Size size;
				ColorDepth depth;
				bool hasInfo = false;

				if (iconInfo.hbmColor)
				{
					hasInfo = GetBitmapInfo(iconInfo.hbmColor, size, depth);
				}
				else if (iconInfo.hbmMask)
				{
					hasInfo = GetBitmapInfo(iconInfo.hbmMask, size, depth);
				}

				if (hasInfo && std::invoke(unshareFunc))
				{
					wxGDIImageRefData* refData = gdiImage.GetGDIImageData();
					refData->m_handle = handle;
					refData->m_width = size.GetWidth();
					refData->m_height = size.GetHeight();
					refData->m_depth = depth.GetValue();

					return true;
				}
			}

			// Delete the handle if we can't attach it
			::DeleteObject(handle);
		}
		return false;
	}
}
