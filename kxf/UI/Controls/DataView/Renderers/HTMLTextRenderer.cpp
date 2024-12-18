#include "kxf-pch.h"
#include "HTMLTextRenderer.h"
#include "../View.h"
#include "../Node.h"
#include "../Column.h"
#include "kxf/UI/Controls/HTMLWindow.h"
#include "kxf/Drawing/GDIRenderer/GDIContext.h"
#include "kxf/Drawing/GDIRenderer/GDIAction.h"
#include <wx/html/htmprint.h>

namespace
{
	// For some unknown reason scale of '1.0' renders text too large.
	// Scaling it to '0.8' fixes this.
	constexpr double g_UserScale = 0.8;

	class DCUserScaleSaver final
	{
		private:
			kxf::GDIContext& m_DC;
			kxf::SizeD m_UserScale;

		public:
			DCUserScaleSaver(kxf::GDIContext& dc)
				:m_DC(dc), m_UserScale(dc.GetUserScale())
			{
			}
			~DCUserScaleSaver()
			{
				m_DC.SetUserScale(m_UserScale);
			}
	};
}

namespace kxf::UI::DataView
{
	bool HTMLTextRenderer::SetDisplayValue(Any value)
	{
		m_Value = {};
		m_ContentHTML.clear();

		if (m_Value.FromAny(value))
		{
			m_ContentHTML = UI::HTMLWindow::ProcessPlainText(m_Value.GetText());
			return true;
		}
		return false;
	}
	ToolTip HTMLTextRenderer::CreateToolTip() const
	{
		return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
	}

	void HTMLTextRenderer::PrepareRenderer(wxHtmlDCRenderer& htmlRenderer, GDIContext& dc, const Rect& cellRect) const
	{
		htmlRenderer.SetDC(&dc.ToWxDC(), g_UserScale * m_PixelScale, g_UserScale * m_FontScale);

		// Size
		Size size = cellRect.GetSize();
		size.IncTo(GetCellSize());
		htmlRenderer.SetSize(size.GetWidth(), size.GetHeight());

		// Set HTML
		htmlRenderer.SetHtmlText(m_ContentHTML);

		// Setup fonts. This needs to be done after a call to 'wxHtmlDCRenderer::SetHtmlText',
		// otherwise text scales really weirdly.
		int pointSize = 0;
		String normalFace;
		String fixedFace;
		if (UI::HTMLWindow::SetupFontsUsing(dc.GetFont(), normalFace, fixedFace, pointSize))
		{
			htmlRenderer.SetFonts(normalFace, fixedFace);
			htmlRenderer.SetStandardFonts(pointSize, normalFace, fixedFace);
		}
	}
	void HTMLTextRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		if (m_Value.HasText())
		{
			IGraphicsContext& gc = GetGraphicsContext();
			gc.DrawGDI(cellRect, [&](GDIContext& dc)
			{
				DCUserScaleSaver userScaleSaver(dc);
				GDIAction::Clip clip(dc, cellRect.Clone().Deflate(0, GetRenderEngine().FromDIPY(2)));

				// Render text
				wxHtmlDCRenderer htmlRenderer;
				PrepareRenderer(htmlRenderer, dc, cellRect);
				htmlRenderer.Render(cellRect.GetX(), cellRect.GetY(), m_VisibleCellFrom, m_VisibleCellTo);
			});
		}
	}
	Size HTMLTextRenderer::GetCellSize() const
	{
		if (m_Value.HasText())
		{
			return GetRenderEngine().GetTextExtent(m_Value.GetText());
		}
		return {};
	}
}
