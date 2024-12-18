#pragma once
#include "Common.h"
#include "../IClusterMapWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::WXUI
{
	class ClusterMap;
}

namespace kxf::Widgets
{
	class KXF_API ClusterMap: public RTTI::Implementation<ClusterMap, Private::BasicWxWidget<ClusterMap, WXUI::ClusterMap, IClusterMapWidget>, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			ClusterMap();
			~ClusterMap();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IClusterMapWidget
			size_t HitTest(const Point& pos) const override;
			Rect GetItemRect(const Point& pos) const override;
			Rect GetItemRect(size_t index) const override;

			size_t GetItemCount() const override;
			void SetItemCount(size_t count) override;

			int GetItemSize() const override;
			void SetItemSize(int size, int spacing = 0) override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
