#include "kxf-pch.h"
#include "GDIImageList.h"
#include "GDIContext.h"
#include "../Color.h"
#include "kxf/System/COM.h"

#include <Shlobj.h>
#include <commoncontrols.h>
#include "kxf/Win32/LinkLibs-GUI.h"
#include "kxf/Win32/UndefMacros.h"

namespace
{
	using namespace kxf;

	constexpr bool g_UseMask = false;

	constexpr FlagSet<uint32_t> MapDrawMode(FlagSet<ImageListFlag> drawMode) noexcept
	{
		FlagSet<uint32_t> nativeDrawMode = ILD_IMAGE|ILD_NORMAL;
		nativeDrawMode.Add(ILD_TRANSPARENT, drawMode & ImageListFlag::Transparent);
		nativeDrawMode.Add(ILD_SELECTED, drawMode & ImageListFlag::Selected);
		nativeDrawMode.Add(ILD_FOCUS, drawMode & ImageListFlag::Focused);

		return nativeDrawMode;
	}
	HIMAGELIST ToHImageList(WXHIMAGELIST handle) noexcept
	{
		return reinterpret_cast<HIMAGELIST>(handle);
	}
}

namespace kxf
{
	wxIMPLEMENT_DYNAMIC_CLASS(GDIImageList, wxImageList);

	// GDIImageList
	void GDIImageList::OnCreate(int width, int height, bool mask, int initialCount) noexcept
	{
		// Replicate wxImageList flags algorithm
		m_Flags = ILC_COLOR32;

		// Common Controls before v6 always uses masks as it doesn't support alpha
		if (mask || wxApp::GetComCtl32Version() < 600)
		{
			m_Flags |= ILC_MASK;
		}
	}
	bool GDIImageList::DoDraw(GDIContext& dc, int index, const Rect& rect, FlagSet<ImageListFlag> flags, int overlayIndex) noexcept
	{
		Size size = rect.GetSize();
		size.SetDefaults({0, 0});

		auto nativeDrawMode = MapDrawMode(flags);
		nativeDrawMode.Add(INDEXTOOVERLAYMASK(overlayIndex), overlayIndex > 0);

		return ::ImageList_DrawEx(ToHImageList(m_hImageList), index, static_cast<HDC>(dc.GetHandle()), rect.GetX(), rect.GetY(), size.GetWidth(), size.GetHeight(), CLR_NONE, CLR_NONE, *nativeDrawMode);
	}

	GDIImageList::GDIImageList() noexcept
	{
		OnCreate(wxDefaultCoord, wxDefaultCoord, g_UseMask, -1);
	}
	GDIImageList::GDIImageList(int width, int height, int initialCount) noexcept
		:wxImageList(width, height, g_UseMask, initialCount)
	{
		OnCreate(width, height, g_UseMask, initialCount);
	}

	// IGDIObject
	bool GDIImageList::IsNull() const
	{
		return m_hImageList != nullptr;
	}
	bool GDIImageList::IsSameAs(const IGDIObject& other) const
	{
		return m_hImageList == other.GetHandle();
	}
	std::shared_ptr<IGDIObject> GDIImageList::CloneGDIObject() const
	{
		const size_t count = GetImageCount();
		const auto size = GetSize();
		auto clone = std::make_shared<GDIImageList>(size.GetWidth(), size.GetHeight(), count);
		clone->m_Flags = m_Flags;

		for (size_t i = 0; i < count; i++)
		{
			clone->Add(GetIcon(i));
		}
		return clone;
	}

	void* GDIImageList::GetHandle() const
	{
		return m_hImageList;
	}
	void* GDIImageList::DetachHandle()
	{
		void* handle = m_hImageList;
		m_hImageList = nullptr;
		m_size = wxDefaultSize;

		return handle;
	}
	void GDIImageList::AttachHandle(void* handle)
	{
		AllocExclusive();

		if (m_hImageList)
		{
			::ImageList_Destroy(ToHImageList(m_hImageList));
			m_hImageList = nullptr;
		}

		if (handle)
		{
			m_hImageList = static_cast<WXHIMAGELIST>(handle);
			ImageList_GetIconSize(ToHImageList(m_hImageList), &m_size.x, &m_size.y);
		}
		else
		{
			m_size = wxDefaultSize;
		}
	}

	// GDIImageList
	bool GDIImageList::HasMask() const noexcept
	{
		return m_Flags & ILC_MASK;
	}
	COMPtr<IImageList2> GDIImageList::QueryNativeInterface() const noexcept
	{
		COMPtr<IImageList2> imageList;
		if (HResult(::HIMAGELIST_QueryInterface(ToHImageList(m_hImageList), __uuidof(IImageList2), imageList.GetAddress())))
		{
			return imageList;
		}
		return nullptr;
	}

	bool GDIImageList::Create(int width, int height, int initialCount) noexcept
	{
		const bool result = wxImageList::Create(width, height, g_UseMask, initialCount);
		OnCreate(width, height, g_UseMask, initialCount);
		return result;
	}
	bool GDIImageList::Create(const Size& size, int initialCount) noexcept
	{
		return Create(size.GetWidth(), size.GetHeight(), initialCount);
	}
	bool GDIImageList::Clear() noexcept
	{
		return wxImageList::RemoveAll();
	}
	bool GDIImageList::RemoveAll() noexcept
	{
		return wxImageList::RemoveAll();
	}
	bool GDIImageList::Remove(int index) noexcept
	{
		if (index > 0)
		{
			return wxImageList::Remove(index);
		}
		return false;
	}

	int GDIImageList::Add(const GDIBitmap& bitmap)
	{
		return wxImageList::Add(bitmap.ToWxBitmap(), wxNullBitmap);
	}
	int GDIImageList::Add(const GDIIcon& icon)
	{
		return wxImageList::Add(icon.ToWxIcon());
	}
	int GDIImageList::Add(const BitmapImage& image)
	{
		return Add(image.ToGDIBitmap());
	}

	bool GDIImageList::Replace(int index, const BitmapImage& image)
	{
		return wxImageList::Replace(index, image.ToGDIBitmap().ToWxBitmap());
	}
	bool GDIImageList::Replace(int index, const GDIIcon& icon)
	{
		return wxImageList::Replace(index, icon.ToWxIcon());
	}
	bool GDIImageList::Replace(int index, const GDIBitmap& bitmap)
	{
		return wxImageList::Replace(index, bitmap.ToWxBitmap(), wxNullBitmap);
	}

	BitmapImage GDIImageList::GetImage(int index) const
	{
		return GetBitmap(index).ToBitmapImage();
	}
	GDIIcon GDIImageList::GetIcon(int index) const
	{
		return wxImageList::GetIcon(index);
	}
	GDIBitmap GDIImageList::GetBitmap(int index) const
	{
		return GDIIcon(wxImageList::GetIcon(index)).ToGDIBitmap();
	}

	Color GDIImageList::GetBackgroundColor() const noexcept
	{
		return Color::FromCOLORREF(::ImageList_GetBkColor(ToHImageList(m_hImageList)));
	}
	void GDIImageList::SetBackgroundColor(const Color& color) noexcept
	{
		::ImageList_SetBkColor(ToHImageList(m_hImageList), color ? color.GetCOLORREF() : CLR_NONE);
	}
	bool GDIImageList::SetOverlayImage(int index, int overlayIndex) noexcept
	{
		return ::ImageList_SetOverlayImage(ToHImageList(m_hImageList), index, overlayIndex);
	}

	GDIImageList& GDIImageList::operator=(GDIImageList&& other) noexcept
	{
		if (m_hImageList)
		{
			::ImageList_Destroy(ToHImageList(m_hImageList));
			m_hImageList = nullptr;
		}

		m_hImageList = std::exchange(other.m_hImageList, nullptr);
		m_size = std::exchange(other.m_size, Size::UnspecifiedSize());
		m_Flags = std::exchange(other.m_Flags, 0);

		return *this;
	}
}
