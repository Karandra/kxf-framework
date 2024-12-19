#include "kxf-pch.h"
#include "CellRendererHelper.h"
#include "Column.h"
#include "../../IDataViewWidget.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"
#include "kxf-gui/Drawing/IRendererNative.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIWindowContext.h"
#include "wx/generic/private/markuptext.h"

namespace
{
	using MarkupMode = kxf::IDataViewCellRenderer::MarkupMode;
	using MarkupFlags = decltype(wxMarkupText::Render_Default);
}

namespace kxf::DataView::Markup
{
	class TextOnly final: public wxItemMarkupText
	{
		public:
			TextOnly(const String& markup)
				:wxItemMarkupText(markup)
			{
			}

		public:
			Size GetTextExtent(GDIContext& dc) const
			{
				return Size(Measure(dc.AsWXDC()));
			}
	};
	class WithMnemonics final: public wxMarkupText
	{
		public:
			WithMnemonics(const String& markup)
				:wxMarkupText(markup)
			{
			}

		public:
			Size GetTextExtent(GDIContext& dc) const
			{
				return Size(Measure(dc.AsWXDC()));
			}
	};

	template<MarkupMode mode>
	auto Create(const String& string = {})
	{
		if constexpr(mode == MarkupMode::TextOnly)
		{
			return TextOnly(string);
		}
		else if constexpr(mode == MarkupMode::WithMnemonics)
		{
			return WithMnemonics(string);
		}
	}

	template<class T>
	Size GetTextExtent(T& markup, GDIContext& dc)
	{
		return Size(markup.Measure(dc.AsWXDC()));
	}

	Size GetTextExtent(MarkupMode mode, GDIContext& dc, const String& string)
	{
		switch (mode)
		{
			case MarkupMode::TextOnly:
			{
				return Create<MarkupMode::TextOnly>(string).GetTextExtent(dc);
			}
			case MarkupMode::WithMnemonics:
			{
				return Create<MarkupMode::WithMnemonics>(string).GetTextExtent(dc);
			}
		};
		return {0, 0};
	}

	template<class T>
	void DrawText(T& markup, wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<MarkupFlags> flags, wxEllipsizeMode ellipsizeMode)
	{
		if constexpr(std::is_same_v<T, WithMnemonics>)
		{
			markup.Render(dc.AsWXDC(), rect, flags.ToInt());
		}
		else
		{
			markup.Render(window, dc.AsWXDC(), rect, flags.ToInt(), ellipsizeMode);
		}
	}

	void DrawText(MarkupMode mode, const String& string, wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<MarkupFlags> flags, wxEllipsizeMode ellipsizeMode)
	{
		switch (mode)
		{
			case MarkupMode::TextOnly:
			{
				auto markup = Create<MarkupMode::TextOnly>(string);
				return DrawText(markup, window, dc, rect, flags, ellipsizeMode);
			}
			case MarkupMode::WithMnemonics:
			{
				auto markup = Create<MarkupMode::WithMnemonics>(string);
				return DrawText(markup, window, dc, rect, flags, ellipsizeMode);
			}
		};
	}
}

namespace kxf::DataView
{
	CellRendererHelper::CellRendererHelper(const IDataViewCellRenderer::RenderInfo& renderInfo)
		:m_Widget(renderInfo.Column.GetOwningWdget()), m_Context(*renderInfo.GraphicsContext), m_RenderInfo(renderInfo)
	{
	}

	float CellRendererHelper::GetInterTextSpacing() const
	{
		return 2.0f;
	}
	float CellRendererHelper::CalcCenter(int cellSize, int itemSize) const
	{
		return (cellSize - itemSize) / 2.0f;
	}
	Size CellRendererHelper::FromDIP(const Size& size) const
	{
		return m_Widget.FromDIP(size);
	}

	size_t CellRendererHelper::FindFirstLineBreak(const String& string, String* before) const
	{
		for (size_t i = 0; i < string.size(); i++)
		{
			const XChar c = string[i];
			if (c == '\r' || c == '\n')
			{
				if (before)
				{
					*before = string.SubLeft(i);
				}

				return i;
			}
		}
		return String::npos;
	}
	FlagSet<NativeWidgetFlag> CellRendererHelper::GetControlFlags(CellState cellState, const Column* column) const
	{
		FlagSet<NativeWidgetFlag> flags;
		flags.Add(NativeWidgetFlag::Disabled, !m_RenderInfo.Attributes.Options().ContainsOption(CellStyle::Enabled));
		flags.Add(NativeWidgetFlag::Editable, m_RenderInfo.Attributes.Options().ContainsOption(CellStyle::Editable));
		flags.Add(NativeWidgetFlag::Selected|NativeWidgetFlag::Pressed, cellState.IsSelected());
		flags.Add(NativeWidgetFlag::Current|NativeWidgetFlag::Focused, column && cellState.IsHotTracked() && column->IsHotTracked());

		return flags;
	}
	String CellRendererHelper::StripMarkup(const String& markup) const
	{
		// Stub for now. Need proper markup removal algorithm
		return wxControl::RemoveMnemonics(markup);
	}

	Size CellRendererHelper::GetTextExtent(const String& string) const
	{
		if (m_RenderInfo.MarkupMode != MarkupMode::Disabled)
		{
			SizeF extent;
			m_Context.DrawGDI({0, 0, 1, 1}, [&](GDIContext& dc)
			{
				if (!m_RenderInfo.Attributes.FontOptions().IsDefault())
				{
					dc.SetFont(m_RenderInfo.Attributes.GetEffectiveFont(dc.GetFont().ToFont()));
				}
				extent = Markup::GetTextExtent(m_RenderInfo.MarkupMode, dc, string);
			});
			return extent;
		}
		else
		{
			auto GetEffectiveFontIfNeeded = [&]() -> Font
			{
				if (!m_RenderInfo.Attributes.FontOptions().IsDefault())
				{
					if (auto font = m_Context.GetFont())
					{
						return m_RenderInfo.Attributes.GetEffectiveFont(font->ToFont());
					}
					else
					{
						return m_RenderInfo.Attributes.GetEffectiveFont(m_Widget.GetFont());
					}
				}
				return {};
			};
			auto MeasureString = [&](const String& text, const Font& font = {})
			{
				if (font)
				{
					auto gcFont = m_Context.GetRenderer().CreateFont(font);
					return m_Context.GetTextExtent(text, *gcFont).ConvertCeil<Size>();
				}
				else
				{
					return m_Context.GetTextExtent(text).ConvertCeil<Size>();
				}
			};

			const size_t lineBreakPos = FindFirstLineBreak(string);
			if (lineBreakPos != String::npos)
			{
				return MeasureString(string.SubLeft(lineBreakPos), GetEffectiveFontIfNeeded());
			}
			else
			{
				return MeasureString(string, GetEffectiveFontIfNeeded());
			}
		}
	}
	bool CellRendererHelper::DrawText(const Rect& cellRect, const String& string, int offsetX)
	{
		if (!string.IsEmpty())
		{
			Rect textRect = cellRect;
			textRect.X() += offsetX;
			textRect.Width() -= offsetX;

			FlagSet<MarkupFlags> flags;
			flags.Add(wxMarkupText::Render_ShowAccels, m_RenderInfo.MarkupMode == MarkupMode::WithMnemonics && m_RenderInfo.Attributes.Options().ContainsOption(CellStyle::ShowAccelerators));

			if (m_RenderInfo.MarkupMode != MarkupMode::Disabled)
			{
				m_Context.DrawGDI({0, 0, 1, 1}, [&](GDIContext& dc)
				{
					Markup::DrawText(m_RenderInfo.MarkupMode, string, m_Widget.GetWxWindow(), dc, textRect, flags, static_cast<wxEllipsizeMode>(m_RenderInfo.EllipsizeMode));
				});
			}
			else
			{
				auto Ellipsize = [&](const String& label)
				{
					return m_Context.EllipsizeText(label, textRect.GetWidth(), m_RenderInfo.EllipsizeMode);
				};

				String firstLine;
				const size_t lineBreakPos = FindFirstLineBreak(string, &firstLine);
				if (lineBreakPos != String::npos)
				{
					m_Context.DrawText(Ellipsize(firstLine), textRect.GetPosition());
				}
				else
				{
					m_Context.DrawText(Ellipsize(string), textRect.GetPosition());
				}
			}
			return true;
		}
		return false;
	}

	bool CellRendererHelper::DrawBitmap(const Rect& cellRect, const BitmapImage& bitmap, int reservedWidth)
	{
		if (bitmap)
		{
			const Rect rect = {cellRect.GetPosition(), bitmap.GetSize()};
			const bool isEnabled = m_RenderInfo.Attributes.Options().ContainsOption(CellStyle::Enabled);

			m_Context.DrawTexture(isEnabled ? bitmap : bitmap.ConvertToDisabled(), rect);
			return true;
		}
		else if (reservedWidth > 0)
		{
			return true;
		}
		return false;
	}
	int CellRendererHelper::DrawBitmapWithText(const Rect& cellRect, int offsetX, const String& text, const BitmapImage& bitmap, bool centerTextV, int reservedWidth)
	{
		if (bitmap || reservedWidth > 0)
		{
			DrawBitmap(Rect(cellRect.GetX() + offsetX, cellRect.GetY(), cellRect.GetWidth() - offsetX, cellRect.GetHeight()), bitmap, reservedWidth);
			offsetX += (reservedWidth > 0 ? reservedWidth : bitmap.GetSize().GetWidth()) + FromDIPX(GetInterTextSpacing());
		}
		if (!text.IsEmpty())
		{
			if (bitmap && centerTextV)
			{
				const Rect textRect = CenterTextInside(cellRect, GetTextExtent(text));
				DrawText(textRect, text, offsetX);
			}
			else
			{
				DrawText(cellRect, text, offsetX);
			}
		}
		return offsetX;
	}
	void CellRendererHelper::DrawProgressBar(const Rect& cellRect, CellState cellState, int value, int range, ProgressMeterState state, Color* averageBackgroundColor)
	{
		auto flags = GetControlFlags(cellState);
		flags.Add(NativeWidgetFlag::Paused, state == ProgressMeterState::Pause);
		flags.Add(NativeWidgetFlag::Error, state == ProgressMeterState::Error);

		IRendererNative::Get().DrawProgressMeter(m_Widget.GetWxWindow(), m_Context, cellRect, value, range, flags);
	}

	Size CellRendererHelper::GetToggleSize() const
	{
		return IRendererNative::Get().GetCheckBoxSize(m_Widget.GetWxWindow());
	}
	Size CellRendererHelper::DrawToggle(const Rect& cellRect, CellState cellState, CheckWidgetValue toggleState, CheckWidgetType toggleType)
	{
		auto flags = GetControlFlags(cellState);
		flags.Add(NativeWidgetFlag::Checkable);
		flags.Add(NativeWidgetFlag::Checked, toggleState == CheckWidgetValue::Checked);
		flags.Add(NativeWidgetFlag::Indeterminate, toggleState == CheckWidgetValue::Indeterminate);
		flags.Add(NativeWidgetFlag::Radio, toggleType != CheckWidgetType::Button && toggleState != CheckWidgetValue::Indeterminate);

		// Ensure that the check boxes always have at least the minimal required size,
		// otherwise DrawCheckBox() doesn't really work well. If this size is greater than
		// the cell size, the checkbox will be truncated but this is a lesser evil.
		Rect toggleRect = cellRect;
		Size size = toggleRect.GetSize();
		size.IncTo(GetToggleSize());
		toggleRect.SetSize(size);

		IRendererNative::Get().DrawCheckBox(m_Widget.GetWxWindow(), m_Context, toggleRect, flags);
		return toggleRect.GetSize();
	}
}
