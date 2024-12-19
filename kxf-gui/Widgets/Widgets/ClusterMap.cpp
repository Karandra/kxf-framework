#include "kxf-pch.h"
#include "ClusterMap.h"
#include "WXUI/ClusterMap.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"

namespace kxf::Widgets
{
	// Button
	ClusterMap::ClusterMap()
	{
		InitializeWxWidget();
	}
	ClusterMap::~ClusterMap() = default;

	// IWidget
	bool ClusterMap::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IClusterMapWidget
	size_t ClusterMap::HitTest(const Point& pos) const
	{
		return Get()->HitTest(pos);
	}
	Rect ClusterMap::GetItemRect(const Point& pos) const
	{
		return Get()->GetItemRect(pos);
	}
	Rect ClusterMap::GetItemRect(size_t index) const
	{
		return Get()->GetItemRect(index);
	}

	size_t ClusterMap::GetItemCount() const
	{
		return Get()->GetItemCount();
	}
	void ClusterMap::SetItemCount(size_t count)
	{
		return Get()->SetItemCount(count);
	}

	int ClusterMap::GetItemSize() const
	{
		return Get()->GetItemSize();
	}
	void ClusterMap::SetItemSize(int size, int spacing)
	{
		return Get()->SetItemSize(size, spacing);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> ClusterMap::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
