#pragma once
#include "Common.h"
#include "IGraphicsContext.h"

namespace kxf::GraphicsAction
{
	class ChangeAntialiasMode final
	{
		private:
			IGraphicsContext& m_GC;
			const AntialiasMode m_OriginalMode = AntialiasMode::None;

		public:
			ChangeAntialiasMode(IGraphicsContext& gc, AntialiasMode newMode)
				:m_GC(gc), m_OriginalMode(gc.GetAntialiasMode())
			{
				m_GC.SetAntialiasMode(newMode);
			}
			~ChangeAntialiasMode()
			{
				m_GC.SetAntialiasMode(m_OriginalMode);
			}
	};

	class ChangeCompositionMode final
	{
		private:
			IGraphicsContext& m_GC;
			const CompositionMode m_OriginalMode = CompositionMode::None;

		public:
			ChangeCompositionMode(IGraphicsContext& gc, CompositionMode newMode)
				:m_GC(gc), m_OriginalMode(gc.GetCompositionMode())
			{
				m_GC.SetCompositionMode(newMode);
			}
			~ChangeCompositionMode()
			{
				m_GC.SetCompositionMode(m_OriginalMode);
			}
	};

	class ChangeInterpolationQuality final
	{
		private:
			IGraphicsContext& m_GC;
			const InterpolationQuality m_OriginalQuality = InterpolationQuality::None;

		public:
			ChangeInterpolationQuality(IGraphicsContext& gc, InterpolationQuality newQuality)
				:m_GC(gc), m_OriginalQuality(gc.GetInterpolationQuality())
			{
				m_GC.SetInterpolationQuality(newQuality);
			}
			~ChangeInterpolationQuality()
			{
				m_GC.SetInterpolationQuality(m_OriginalQuality);
			}
	};

	class BeginLayer final
	{
		private:
			IGraphicsContext& m_GC;

		public:
			BeginLayer(IGraphicsContext& gc, float opacity)
				:m_GC(gc)
			{
				m_GC.BeginLayer(opacity);
			}
			~BeginLayer()
			{
				m_GC.EndLayer();
			}
	};

	class PushState final
	{
		private:
			IGraphicsContext& m_GC;

		public:
			PushState(IGraphicsContext& gc)
				:m_GC(gc)
			{
				m_GC.PushState();
			}
			~PushState()
			{
				m_GC.PopState();
			}
	};

	class StartDocument final
	{
		private:
			IGraphicsContext& m_GC;
			const bool m_Result = false;

		public:
			StartDocument(IGraphicsContext& gc, const String& message)
				:m_GC(gc), m_Result(gc.StartDocument(message))
			{
			}
			~StartDocument()
			{
				m_GC.EndDocument();
			}

		public:
			operator bool() const
			{
				return m_Result;
			}
	};

	class Clip final
	{
		private:
			IGraphicsContext& m_GC;
			RectF m_OldClip;

			bool m_Clipped = false;
			bool m_SupportStates = false;

		private:
			void DoInitialClip(const Rect& rect)
			{
				if (m_SupportStates)
				{
					m_GC.PushState();
				}
				else
				{
					m_OldClip = m_GC.GetClipBox();
				}
				m_GC.ClipBoxRegion(rect);
				m_Clipped = true;
			}
			void DoAddClip(const Rect& rect)
			{
				m_GC.ClipBoxRegion(rect);
			}
			void DoResetClip()
			{
				if (m_Clipped)
				{
					if (m_SupportStates)
					{
						m_GC.PopState();
					}
					else
					{
						if (m_OldClip.IsEmpty())
						{
							m_GC.ResetClipRegion();
						}
						else
						{
							m_GC.ClipBoxRegion(m_OldClip);
						}
					}
					m_Clipped = false;
				}
			}

		public:
			Clip(IGraphicsContext& gc)
				:m_GC(gc), m_SupportStates(m_GC.GetSupportedFeatures().Contains(GraphicsContextFeature::States))
			{
			}
			Clip(IGraphicsContext& gc, const Rect& rect)
				:Clip(gc)
			{
				DoInitialClip(rect);
			}
			~Clip()
			{
				DoResetClip();
			}

		public:
			void Add(const Rect& rect)
			{
				if (m_Clipped)
				{
					DoAddClip(rect);
				}
				else
				{
					DoInitialClip(rect);
				}
			}
			void Reset()
			{
				DoResetClip();
			}
	};

	class ChangeFont final
	{
		private:
			IGraphicsContext& m_GC;
			std::shared_ptr<IGraphicsFont> m_Font;

		private:
			void SaveOldFont()
			{
				if (!m_Font)
				{
					m_Font = m_GC.GetFont();
				}
			}

		public:
			ChangeFont(IGraphicsContext& gc)
				:m_GC(gc)
			{
			}
			ChangeFont(IGraphicsContext& gc, std::shared_ptr<IGraphicsFont> font)
				:m_GC(gc), m_Font(gc.GetFont())
			{
				m_GC.SetFont(std::move(font));
			}
			~ChangeFont()
			{
				if (m_Font)
				{
					m_GC.SetFont(std::move(m_Font));
				}
			}

		public:
			void Set(std::shared_ptr<IGraphicsFont> font)
			{
				SaveOldFont();
				m_GC.SetFont(std::move(font));
			}
	};

	class ChangeFontBrush final
	{
		private:
			IGraphicsContext& m_GC;
			std::shared_ptr<IGraphicsBrush> m_Brush;

		private:
			void SaveOldBrush()
			{
				if (!m_Brush)
				{
					m_Brush = m_GC.GetFontBrush();
				}
			}

		public:
			ChangeFontBrush(IGraphicsContext& gc)
				:m_GC(gc)
			{
			}
			ChangeFontBrush(IGraphicsContext& gc, std::shared_ptr<IGraphicsBrush> brush)
				:m_GC(gc), m_Brush(gc.GetFontBrush())
			{
				m_GC.SetFontBrush(std::move(brush));
			}
			ChangeFontBrush(IGraphicsContext& gc, const Color& color)
				:m_GC(gc), m_Brush(gc.GetFontBrush())
			{
				m_GC.SetFontBrush(m_GC.GetRenderer().CreateSolidBrush(color));
			}
			~ChangeFontBrush()
			{
				if (m_Brush)
				{
					m_GC.SetFontBrush(std::move(m_Brush));
				}
			}

		public:
			void Set(std::shared_ptr<IGraphicsBrush> brush)
			{
				SaveOldBrush();
				m_GC.SetFontBrush(std::move(brush));
			}
			void Set(const Color& color)
			{
				SaveOldBrush();
				m_GC.SetFontBrush(m_GC.GetRenderer().CreateSolidBrush(color));
			}
	};

	class ChangeBrush final
	{
		private:
			IGraphicsContext& m_GC;
			std::shared_ptr<IGraphicsBrush> m_Brush;

		private:
			void SaveOldBrush()
			{
				if (!m_Brush)
				{
					m_Brush = m_GC.GetBrush();
				}
			}

		public:
			ChangeBrush(IGraphicsContext& gc)
				:m_GC(gc)
			{
			}
			ChangeBrush(IGraphicsContext& gc, std::shared_ptr<IGraphicsBrush> brush)
				:m_GC(gc), m_Brush(gc.GetBrush())
			{
				m_GC.SetBrush(std::move(brush));
			}
			ChangeBrush(IGraphicsContext& gc, const Color& color)
				:m_GC(gc), m_Brush(gc.GetBrush())
			{
				m_GC.SetBrush(m_GC.GetRenderer().CreateSolidBrush(color));
			}
			~ChangeBrush()
			{
				if (m_Brush)
				{
					m_GC.SetBrush(std::move(m_Brush));
				}
			}

		public:
			void Set(std::shared_ptr<IGraphicsBrush> brush)
			{
				SaveOldBrush();
				m_GC.SetBrush(std::move(brush));
			}
			void Set(const Color& color)
			{
				SaveOldBrush();
				m_GC.SetBrush(m_GC.GetRenderer().CreateSolidBrush(color));
			}
	};

	class ChangePen final
	{
		private:
			IGraphicsContext& m_GC;
			std::shared_ptr<IGraphicsPen> m_Pen;

		private:
			void SaveOldPen()
			{
				if (!m_Pen)
				{
					m_Pen = m_GC.GetPen();
				}
			}

		public:
			ChangePen(IGraphicsContext& gc)
				:m_GC(gc)
			{
			}
			ChangePen(IGraphicsContext& gc, std::shared_ptr<IGraphicsPen> pen)
				:m_GC(gc), m_Pen(gc.GetPen())
			{
				m_GC.SetPen(std::move(pen));
			}
			ChangePen(IGraphicsContext& gc, const Color& color, float width = 1.0f)
				:m_GC(gc), m_Pen(gc.GetPen())
			{
				m_GC.SetPen(m_GC.GetRenderer().CreatePen(color, width));
			}
			~ChangePen()
			{
				if (m_Pen)
				{
					m_GC.SetPen(std::move(m_Pen));
				}
			}

		public:
			void Set(std::shared_ptr<IGraphicsPen> pen)
			{
				SaveOldPen();
				m_GC.SetPen(std::move(pen));
			}
			void Set(const Color& color, float width = 1.0f)
			{
				SaveOldPen();
				m_GC.SetPen(m_GC.GetRenderer().CreatePen(color, width));
			}
	};
}
