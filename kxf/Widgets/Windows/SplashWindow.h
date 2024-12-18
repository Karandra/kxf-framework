#pragma once
#include "kxf/Widgets/Common.h"
#include "Frame.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"

namespace kxf::UI
{
	enum class SplashWindowStyle: uint32_t
	{
		None = 0,

		CenterOnParent = 1 << 0,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::SplashWindowStyle);
	kxf_FlagSet_Extend(UI::SplashWindowStyle, UI::WindowStyle);
}

namespace kxf::UI
{
	class KXF_API SplashWindow: public WindowRefreshScheduler<Frame>
	{
		public:
			static constexpr FlagSet<SplashWindowStyle> DefaultStyle = SplashWindowStyle::None;

		private:
			GDIBitmap m_Bitmap;
			uint8_t m_Alpha = 255;
		
			wxTimer m_Timer;
			TimeSpan m_Timeout;
			FlagSet<SplashWindowStyle> m_Style;

		private:
			void OnTimer(wxTimerEvent& event);
			void OnSize(wxSizeEvent& event);

		protected:
			virtual void DoSetSplash(const GDIBitmap& bitmap, const Size& size);
			virtual bool DoUpdateSplash();
			virtual void DoCenterWindow();

		public:
			SplashWindow() = default;
			SplashWindow(wxWindow* parent,
						 const GDIBitmap& bitmap,
						 TimeSpan timeout = {},
						 FlagSet<SplashWindowStyle> style = DefaultStyle
			)
			{
				Create(parent, bitmap, timeout, style);
			}
			SplashWindow(wxWindow* parent,
						 const GDIBitmap& bitmap,
						 const Size& size,
						 TimeSpan timeout = {},
						 FlagSet<SplashWindowStyle> style = DefaultStyle
			)
			{
				Create(parent, bitmap, size, timeout, style);
			}
			bool Create(wxWindow* parent,
						const GDIBitmap& bitmap,
						TimeSpan timeout = {},
						FlagSet<SplashWindowStyle> style = DefaultStyle
			)
			{
				return Create(parent, bitmap, bitmap.GetSize(), timeout, style);
			}
			bool Create(wxWindow* parent,
						const GDIBitmap& bitmap,
						const Size& size,
						TimeSpan timeout = {},
						FlagSet<SplashWindowStyle> style = DefaultStyle
			);
			virtual ~SplashWindow();

		public:
			long GetWindowStyleFlag() const override
			{
				return m_Style.ToInt();
			}
			void SetWindowStyleFlag(long style) override
			{
				m_Style.FromInt(style);
				ScheduleRefresh();
			}
		
			bool CanSetTransparent() override
			{
				return true;
			}
			bool SetTransparent(wxByte value) override
			{
				SetSplashAlpha(value);
				return true;
			}

			bool Show(bool show = true) override;
			void Update() override;
			void Refresh(bool eraseBackground = true, const wxRect* rect = nullptr) override;

			const GDIBitmap& GetSplashBitmap() const
			{
				return m_Bitmap;
			}
			void SetSplashBitmap(const GDIBitmap& bitmap, const Size& size = Size::UnspecifiedSize());
		
			uint8_t GetSplashAlpha() const
			{
				return m_Alpha;
			}
			void SetSplashAlpha(uint8_t value);
		
			TimeSpan GetTimeout() const
			{
				return m_Timeout;
			}
			void SetTimeout(TimeSpan timeout)
			{
				m_Timeout = timeout;
			}

			bool UpdateSplash()
			{
				return DoUpdateSplash();
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(SplashWindow);
	};
}
