#include "kxf-pch.h"
#include "Renderer.h"
#include "Column.h"
#include "Node.h"
#include "View.h"
#include "MainWindow.h"
#include "kxf/System/SystemInformation.h"

namespace kxf::UI::DataView
{
	bool Renderer::IsNullRenderer() const
	{
		MainWindow* mainWindow = GetMainWindow();
		return mainWindow ? this == &mainWindow->GetNullRenderer() : false;
	}

	const CellAttribute& Renderer::SetupCellAttributes(CellState cellState)
	{
		// Set up the attributes for this item if it's not empty
		m_Attributes = m_Node->GetCellAttributes(*m_Column, cellState);

		View* view = GetView();
		m_IsViewEnabled = view->IsEnabled();
		m_IsViewFocused = view->HasFocus();

		return m_Attributes;
	}
	void Renderer::SetupCellDisplayValue()
	{
		// Now check if we have a value and remember it for rendering it later.
		// Notice that we do it even if it's null, as the cell should be empty then and not show the last used value.
		SetDisplayValue(m_Node->GetCellDisplayValue(*m_Column));
	}

	void Renderer::CallDrawCellBackground(const Rect& cellRect, CellState cellState, bool noUserBackground)
	{
		auto renderEngine = GetRenderEngine();
		const auto& cellOptions = m_Attributes.Options();
		const auto& cellBGOptions = m_Attributes.BGOptions();

		auto ClipRectIfNeeded = [this, &cellState](const Rect& rect)
		{
			if (GetView()->ContainsWindowStyle(CtrlStyle::VerticalRules))
			{
				if (m_Column->IsDisplayedLast() && (cellState.IsHotTracked() || cellState.IsSelected()))
				{
					return rect;
				}
				return Rect(rect.GetX(), rect.GetY(), rect.GetWidth() - 1, rect.GetHeight());
			}
			return rect;
		};

		// Special backgrounds
		if (cellBGOptions.ContainsOption(CellBGOption::Header))
		{
			Size offsetSize = Size(GetView()->FromDIP(wxSize(0, 1)));

			Rect buttonRect = cellRect;
			buttonRect.Width() += offsetSize.GetWidth();
			buttonRect.Height() += offsetSize.GetHeight();

			IRendererNative::Get().DrawHeaderButton(GetView(), *m_GC, buttonRect, renderEngine.GetControlFlags(cellState));
			if (!cellState.IsSelected())
			{
				Color lineColor = System::GetColor(SystemColor::Light3D);
				lineColor.SetAlpha8(48);

				auto pen = m_GR->CreatePen(lineColor);
				m_GC->DrawLine(buttonRect.GetLeftBottom(), buttonRect.GetRightBottom(), *pen);
			}
		}
		else if (cellBGOptions.ContainsOption(CellBGOption::Button))
		{
			Rect buttonRect = cellRect.Clone().Inflate(1, 1);
			IRendererNative::Get().DrawPushButton(GetView(), *m_GC, buttonRect, renderEngine.GetControlFlags(cellState));
		}
		else if (cellBGOptions.ContainsOption(CellBGOption::ComboBox))
		{
			IRendererNative::Get().DrawComboBox(GetView(), *m_GC, cellRect, renderEngine.GetControlFlags(cellState));
		}

		// Solid background color
		if (cellOptions.HasBackgroundColor())
		{
			auto brush = m_GR->CreateSolidBrush(m_Attributes.Options().GetBackgroundColor());
			m_GC->DrawRectangle(cellRect, *brush);
		}

		// Call derived class drawing
		if (!noUserBackground)
		{
			DrawCellBackground(cellRect, cellState);
		}
	}
	std::pair<Size, Rect> Renderer::CallDrawCellContent(const Rect& cellRect, CellState cellState)
	{
		m_PaintRect = cellRect;
		auto renderEngine = GetRenderEngine();
		const bool isEnabled = m_Attributes.Options().ContainsOption(CellOption::Enabled) && m_IsViewEnabled;
		View* view = GetView();

		// Change text color
		GraphicsAction::ChangeFontBrush changeFontBrush(*m_GC);
		if (Color textColor = m_Attributes.Options().GetForegroundColor(); textColor || !isEnabled)
		{
			if (!textColor)
			{
				textColor = view->GetForegroundColour();
			}
			if (!isEnabled)
			{
				textColor = textColor.MakeDisabled();
			}
			changeFontBrush.Set(textColor);
		}

		// Change font
		GraphicsAction::ChangeFont changeFont(*m_GC);
		if (m_Attributes.FontOptions().RequiresNeedAlteration())
		{
			changeFont.Set(m_GC->GetRenderer().CreateFont(m_Attributes.GetEffectiveFont(view->GetFont())));
		}

		// Adjust the rectangle ourselves to account for the alignment
		const Size cellSize = GetCellSize();
		Rect adjustedCellRect(cellRect);

		// Restrict height to row height
		if (view->ContainsWindowStyle(CtrlStyle::VariableRowHeight))
		{
			adjustedCellRect.SetHeight(GetMainWindow()->GetVariableRowHeight(*m_Node));
		}

		// Take alignment into account only if there is enough space, otherwise show as much contents as possible.
		const auto alignment = m_Attributes.Options().HasAlignment() ? m_Attributes.Options().GetAlignment() : GetEffectiveAlignment();

		if (cellSize.GetWidth() >= 0 && cellSize.GetWidth() < cellRect.GetWidth())
		{
			if (alignment & Alignment::CenterHorizontal)
			{
				adjustedCellRect.X() += renderEngine.CalcCenter(cellRect.GetWidth(), cellSize.GetWidth());
			}
			else if (alignment & Alignment::Right)
			{
				adjustedCellRect.X() += cellRect.GetWidth() - cellSize.GetWidth();
			}
			adjustedCellRect.SetWidth(cellSize.GetWidth());
		}
		if (cellSize.GetHeight() >= 0 && cellSize.GetHeight() < cellRect.GetHeight())
		{
			if (alignment & Alignment::CenterVertical)
			{
				adjustedCellRect.Y() += renderEngine.CalcCenter(cellRect.GetHeight(), cellSize.GetHeight());
			}
			else if (alignment & Alignment::Bottom)
			{
				adjustedCellRect.Y() += cellRect.GetHeight() - cellSize.GetHeight();
			}
			adjustedCellRect.SetHeight(cellSize.GetHeight());
		}

		// Draw highlighting selection
		if (m_Attributes.Options().ContainsOption(CellOption::HighlightItem) && !adjustedCellRect.IsEmpty())
		{
			MainWindow* mainWindow = GetMainWindow();

			Rect highlightRect = Rect(adjustedCellRect).Inflate(2);
			IRendererNative::Get().DrawItemSelectionRect(mainWindow, *m_GC, highlightRect, cellState.ToNativeWidgetFlags(*mainWindow));
		}

		// Call derived class drawing
		DrawCellContent(adjustedCellRect, cellState);
		m_PaintRect = {};

		return {cellSize, adjustedCellRect};
	}
	void Renderer::CallOnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent)
	{
		Any value = OnActivateCell(node, cellRect, mouseEvent);
		if (!value.IsNull() && node.SetCellValue(*m_Column, value))
		{
			GetMainWindow()->OnCellChanged(node, m_Column);
		}
	}

	IGraphicsRenderer& Renderer::GetGraphicsRenderer() const
	{
		if (m_GR)
		{
			return *m_GR;
		}
		return m_Column->GetMainWindow()->GetGraphicsRenderer();
	}

	bool Renderer::HasSolidBackground() const
	{
		return !HasSpecialBackground();
	}
	bool Renderer::HasSpecialBackground() const
	{
		return !m_Attributes.BGOptions().IsDefault();
	}
	Size Renderer::GetCellSize() const
	{
		return Size(0, 0);
	}

	MainWindow* Renderer::GetMainWindow() const
	{
		return m_Column ? m_Column->GetView()->GetMainWindow() : nullptr;
	}
	View* Renderer::GetView() const
	{
		return m_Column ? m_Column->GetView() : nullptr;
	}

	FlagSet<Alignment> Renderer::GetEffectiveAlignment() const
	{
		if (m_Alignment == Alignment::Invalid)
		{
			// If we don't have an explicit alignment ourselves, use that of the
			// column in horizontal direction and default vertical alignment

			FlagSet<Alignment> columnAlignment = m_Column->GetTitleAlignment();
			if (columnAlignment == Alignment::Invalid)
			{
				columnAlignment = Alignment::Left;
			}
			return columnAlignment|Alignment::CenterVertical;
		}
		return m_Alignment;
	}
}
