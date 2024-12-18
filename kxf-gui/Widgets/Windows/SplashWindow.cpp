#include "kxf-pch.h"
#include "SplashWindow.h"
#include "kxf/System/SystemWindow.h" 
#include "kxf/Drawing/GDIRenderer/GDIMemoryContext.h"
#include <wx/rawbmp.h>

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(SplashWindow, wxFrame)

	void SplashWindow::OnTimer(wxTimerEvent& event)
	{
		wxTheApp->ScheduleForDestruction(this);
	}
	void SplashWindow::OnSize(wxSizeEvent& event)
	{
		ScheduleRefresh();
		event.Skip();
	}

	void SplashWindow::DoSetSplash(const GDIBitmap& bitmap, const Size& size)
	{
		m_Bitmap = bitmap;
		SetSize(size.IsFullySpecified() ? size : Size(GetSize()));
	}
	bool SplashWindow::DoUpdateSplash()
	{
		BitmapImage image = m_Bitmap.ToBitmapImage();
		if (image)
		{
			// UpdateLayeredWindow expects premultiplied alpha
			image.InitAlpha();
			const size_t imageSize = image.GetWidth() * image.GetHeight();
			for (size_t i = 0; i < imageSize; i++)
			{
				uint8_t* value = image.GetRawAlpha() + i;
				*value = *value * m_Alpha / 255.0;
			}

			// Scale the image for window size
			if (Size size = Size(GetSize()); size != image.GetSize())
			{
				image.Rescale(size, InterpolationQuality::BestAvailable);
			}
		}

		GDIBitmap bitmap = image.ToGDIBitmap();
		GDIMemoryContext dc(bitmap);

		BLENDFUNCTION blendFunction = {};
		blendFunction.BlendOp = AC_SRC_OVER;
		blendFunction.BlendFlags = 0;
		blendFunction.SourceConstantAlpha = m_Alpha;
		blendFunction.AlphaFormat = AC_SRC_ALPHA;

		POINT pos = {0, 0};
		SIZE size = {bitmap.GetWidth(), bitmap.GetHeight()};
		return ::UpdateLayeredWindow(GetHandle(), nullptr, nullptr, &size, static_cast<HDC>(dc.GetHandle()), &pos, 0, &blendFunction, ULW_ALPHA);
	}
	void SplashWindow::DoCenterWindow()
	{
		if (m_Style & SplashWindowStyle::CenterOnParent)
		{
			CenterOnParent();
		}
		else
		{
			CenterOnScreen();
		}
	}

	bool SplashWindow::Create(wxWindow* parent,
							  const GDIBitmap& bitmap,
							  const Size& size,
							  TimeSpan timeout,
							  FlagSet<SplashWindowStyle> style
	)
	{
		m_Style = style;
		m_Timeout = timeout;

		int frameStyle = wxBORDER_NONE|wxWS_EX_TRANSIENT|wxFRAME_TOOL_WINDOW|wxFRAME_NO_TASKBAR|wxFRAME_SHAPED|wxTRANSPARENT_WINDOW;
		if (parent)
		{
			frameStyle |= wxFRAME_FLOAT_ON_PARENT;
		}

		if (wxFrame::Create(parent, wxID_NONE, {}, Point::UnspecifiedPosition(), size, frameStyle, GetClassInfo()->GetClassName()))
		{
			SystemWindow(GetHandle()).ModWindowStyle(GWL_EXSTYLE, WS_EX_LAYERED|WS_EX_TOOLWINDOW, true);
			m_Timer.Bind(wxEVT_TIMER, &SplashWindow::OnTimer, this);
			m_Timer.Bind(wxEVT_SIZE, &SplashWindow::OnSize, this);

			DoSetSplash(bitmap, size);
			DoUpdateSplash();
			DoCenterWindow();
			return true;
		}
		return false;
	}
	SplashWindow::~SplashWindow()
	{
		m_Timer.Stop();
	}

	bool SplashWindow::Show(bool show)
	{
		const bool result = wxFrame::Show(show);
		if (result && show && m_Timeout.IsPositive())
		{
			m_Timer.StartOnce(m_Timeout.GetMilliseconds());
		}
		return result;
	}
	void SplashWindow::Update()
	{
		DoUpdateSplash();
	}
	void SplashWindow::Refresh(bool eraseBackground, const wxRect* rect)
	{
		DoUpdateSplash();
	}

	void SplashWindow::SetSplashBitmap(const GDIBitmap& bitmap, const Size& size)
	{
		DoSetSplash(bitmap, size);
		DoCenterWindow();
		ScheduleRefresh();
	}
	void SplashWindow::SetSplashAlpha(uint8_t value)
	{
		m_Alpha = value;
		ScheduleRefresh();
	}
}
