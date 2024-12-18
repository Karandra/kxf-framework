#pragma once
#include "Common.h"
#include "CellState.h"
#include "CellAttributes.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Drawing/IRendererNative.h"

namespace kxf::UI::DataView
{
	class Renderer;
}

namespace kxf::UI::DataView
{
	enum class MarkupMode
	{
		Disabled = 0,
		TextOnly,
		WithMnemonics
	};
	enum class ProgressMeterState
	{
		Normal,
		Paused,
		Error
	};
	enum class ToggleState
	{
		None = -2,

		Checked = 1,
		Unchecked = 0,
		Indeterminate = -1,
	};
	enum class ToggleType
	{
		None = -1,

		CheckBox,
		RadioBox,
	};
}

namespace kxf::UI::DataView
{
	class KXF_API RenderEngine
	{
		private:
			Renderer& m_Renderer;

		public:
			RenderEngine(Renderer& renderer)
				:m_Renderer(renderer)
			{
			}

		public:
			int GetInterTextSpacing() const
			{
				return 2;
			}
			int CalcCenter(int cellSize, int itemSize) const;

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

			size_t FindFirstLineBreak(const String& string) const;
			FlagSet<NativeWidgetFlag> GetControlFlags(CellState cellState) const;
			String StripMarkup(const String& markup) const;
			Rect CenterTextInside(const Rect& cellRect, const Size& textExtent) const
			{
				Point pos = cellRect.GetPosition();
				pos.Y() += CalcCenter(cellRect.GetSize().GetHeight(), textExtent.GetHeight());

				return Rect(pos, cellRect.GetSize());
			}

			Size GetTextExtent(const String& string) const;
			Size GetTextExtent(IGraphicsContext& dc, const String& string) const;

			bool DrawText(const Rect& cellRect, CellState cellState, const String& string, int offsetX = 0);
			bool DrawText(IGraphicsContext& gc, const Rect& cellRect, CellState cellState, const String& string, int offsetX = 0);

			bool DrawBitmap(const Rect& cellRect, CellState cellState, const BitmapImage& bitmap, int reservedWidth = -1);
			int DrawBitmapWithText(const Rect& cellRect, CellState cellState, int offsetX, const String& text, const BitmapImage& bitmap, bool centerTextV = false, int reservedWidth = -1);
			void DrawProgressBar(const Rect& cellRect, CellState cellState, int value, int range, ProgressMeterState state = ProgressMeterState::Normal, Color* averageBackgroundColor = nullptr);

			Size GetToggleSize() const;
			Size DrawToggle(IGraphicsContext& gc, const Rect& cellRect, CellState cellState, ToggleState toggleState, ToggleType toggleType);
	};
}
