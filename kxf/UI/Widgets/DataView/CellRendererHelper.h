#pragma once
#include "Common.h"
#include "CellState.h"
#include "CellAttributes.h"
#include "CellRenderer.h"
#include "../../ICheckWidget.h"
#include "../../IProgressMeterWidget.h"

namespace kxf::DataView
{
	class KXF_API CellRendererHelper
	{
		private:
			IWidget& m_Widget;
			IGraphicsContext& m_Context;
			const IDataViewCellRenderer::RenderInfo& m_RenderInfo;

		public:
			CellRendererHelper(const IDataViewCellRenderer::RenderInfo& renderInfo);

		public:
			float GetInterTextSpacing() const;
			float CalcCenter(int cellSize, int itemSize) const;

			Size FromDIP(const Size& size) const;
			Size FromDIP(int x, int y) const
			{
				return FromDIP({x, y});
			}
			int FromDIPX(int x) const
			{
				return FromDIP(x, Geometry::DefaultCoord).GetWidth();
			}
			int FromDIPY(int y) const
			{
				return FromDIP(Geometry::DefaultCoord, y).GetHeight();
			}

			size_t FindFirstLineBreak(const String& string, String* before = nullptr) const;
			FlagSet<NativeWidgetFlag> GetControlFlags(CellState cellState, const Column* column = nullptr) const;
			String StripMarkup(const String& markup) const;
			Rect CenterTextInside(const Rect& cellRect, const Size& textExtent) const
			{
				Point pos = cellRect.GetPosition();
				pos.Y() += CalcCenter(cellRect.GetSize().GetHeight(), textExtent.GetHeight());

				return Rect(pos, cellRect.GetSize());
			}

			Size GetTextExtent(const String& string) const;
			bool DrawText(const Rect& cellRect, const String& string, int offsetX = 0);

			bool DrawBitmap(const Rect& cellRect, const BitmapImage& bitmap, int reservedWidth = -1);
			int DrawBitmapWithText(const Rect& cellRect, int offsetX, const String& text, const BitmapImage& bitmap, bool centerTextV = false, int reservedWidth = -1);
			void DrawProgressBar(const Rect& cellRect, CellState cellState, int value, int range, ProgressMeterState state = ProgressMeterState::Normal, Color* averageBackgroundColor = nullptr);

			Size GetToggleSize() const;
			Size DrawToggle(const Rect& cellRect, CellState cellState, CheckWidgetValue toggleState, CheckWidgetType toggleType);
	};
}
