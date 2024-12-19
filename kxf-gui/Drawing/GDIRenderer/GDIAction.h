#pragma once
#include "Common.h"
#include "GDIContext.h"
#include "GDIPen.h"
#include "GDIBrush.h"
#include "GDIFont.h"

namespace kxf::Drawing
{
	KXF_API Color GetAreaAverageColor(const GDIContext& dc, const Rect& rect);
}

namespace kxf::GDIAction
{
	class KXF_API Clip final
	{
		private:
			GDIContext& m_DC;
			Rect m_OldBox;

		public:
			Clip(GDIContext& dc)
				:m_DC(dc)
			{
			}
			Clip(GDIContext& dc, const Rect& rect)
				:m_DC(dc), m_OldBox(dc.GetClipBox())
			{
			}
			~Clip()
			{
				m_DC.ResetClipRegion();
				m_DC.ClipBoxRegion(m_OldBox);
			}

		public:
			void Add(const Rect& rect)
			{
				m_DC.ClipBoxRegion(rect);
			}
	};

	class ChangeLogicalFunction final
	{
		private:
			GDIContext& m_DC;
			GDILogicalFunction m_OriginalMode = GDILogicalFunction::Clear;

		public:
			ChangeLogicalFunction(GDIContext& dc, GDILogicalFunction newMode)
				:m_DC(dc), m_OriginalMode(dc.GetLogicalFunction())
			{
				m_DC.SetLogicalFunction(newMode);
			}
			~ChangeLogicalFunction()
			{
				m_DC.SetLogicalFunction(m_OriginalMode);
			}
	};

	class ChangeFont final
	{
		private:
			GDIContext& m_DC;
			GDIFont m_OriginalFont;

		public:
			ChangeFont(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangeFont(GDIContext& dc, const GDIFont& font)
				:m_DC(dc), m_OriginalFont(dc.GetFont())
			{
				m_DC.SetFont(font.AsWXFont());
			}
			~ChangeFont()
			{
				if (m_OriginalFont)
				{
					m_DC.SetFont(std::move(m_OriginalFont));
				}
			}

		public:
			void Set(const GDIFont& font)
			{
				if (!m_OriginalFont)
				{
					m_OriginalFont = m_DC.GetFont();
				}
				m_DC.SetFont(font);
			}
	};

	class ChangePen final
	{
		private:
			GDIContext& m_DC;
			GDIPen m_Pen;

		public:
			ChangePen(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangePen(GDIContext& dc, const GDIPen& pen)
				:m_DC(dc), m_Pen(dc.GetPen())
			{
				m_DC.SetPen(pen);
			}
			~ChangePen()
			{
				if (m_Pen)
				{
					m_DC.SetPen(std::move(m_Pen));
				}
			}

		public:
			void Set(const GDIPen& pen)
			{
				if (!m_Pen)
				{
					m_Pen = m_DC.GetPen();
				}
				m_DC.SetPen(pen);
			}
	};

	class ChangeBrush final
	{
		private:
			GDIContext& m_DC;
			GDIBrush m_Brush;

		public:
			ChangeBrush(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangeBrush(GDIContext& dc, const GDIBrush& brush)
				:m_DC(dc), m_Brush(dc.GetBrush())
			{
				m_DC.SetBrush(brush);
			}
			~ChangeBrush()
			{
				if (m_Brush)
				{
					m_DC.SetBrush(std::move(m_Brush));
				}
			}

		public:
			void Set(const GDIBrush& brush)
			{
				if (!m_Brush)
				{
					m_Brush = m_DC.GetBrush();
				}
				m_DC.SetBrush(brush);
			}
	};

	class ChangeBackground final
	{
		private:
			GDIContext& m_DC;
			GDIBrush m_Brush;

		public:
			ChangeBackground(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangeBackground(GDIContext& dc, const GDIBrush& brush)
				:m_DC(dc), m_Brush(dc.GetBackgroundBrush())
			{
				m_DC.SetBackgroundBrush(brush);
			}
			~ChangeBackground()
			{
				if (m_Brush)
				{
					m_DC.SetBackgroundBrush(std::move(m_Brush));
				}
			}

		public:
			void Set(const GDIBrush& brush)
			{
				if (!m_Brush)
				{
					m_Brush = m_DC.GetBrush();
				}
				m_DC.SetBackgroundBrush(brush);
			}
	};

	class ChangeTextForeground final
	{
		private:
			GDIContext& m_DC;
			Color m_OriginalColor;

		public:
			ChangeTextForeground(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangeTextForeground(GDIContext& dc, const Color& color)
				:m_DC(dc), m_OriginalColor(dc.GetTextForeground())
			{
				m_DC.SetTextForeground(color);
			}
			~ChangeTextForeground()
			{
				if (m_OriginalColor)
				{
					m_DC.SetTextForeground(m_OriginalColor);
				}
			}

		public:
			void Set(const Color& color)
			{
				if (!m_OriginalColor)
				{
					m_OriginalColor = m_DC.GetTextForeground();
				}
				m_DC.SetTextForeground(color);
			}
	};

	class ChangeTextBackground final
	{
		private:
			GDIContext& m_DC;
			Color m_OriginalColor;

		public:
			ChangeTextBackground(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangeTextBackground(GDIContext& dc, const Color& color)
				:m_DC(dc), m_OriginalColor(dc.GetTextForeground())
			{
				m_DC.SetTextBackground(color);
			}
			~ChangeTextBackground()
			{
				if (m_OriginalColor)
				{
					m_DC.SetTextBackground(m_OriginalColor);
				}
			}

		public:
			void Set(const Color& color)
			{
				if (!m_OriginalColor)
				{
					m_OriginalColor = m_DC.GetTextBackground();
				}
				m_DC.SetTextBackground(color);
			}
	};
}
