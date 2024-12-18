#include "kxf-pch.h"
#include "Editor.h"
#include "Renderer.h"
#include "Column.h"
#include "View.h"
#include "MainWindow.h"
#include "kxf/Utility/ScopeGuard.h"

namespace kxf::UI::DataView
{
	void Editor::DestroyControl()
	{
		if (m_Control)
		{
			// Remove our event handler first to prevent it from (recursively) calling
			// us again as it would do via a call to 'EndEdit' when the editor
			// loses focus when we hide it below.
			wxEvtHandler* handler = m_Control->PopEventHandler();

			// Hide the control immediately but don't delete it yet as there could be some pending messages for it.
			m_Control->Hide();

			wxTheApp->ScheduleForDestruction(handler);
			wxTheApp->ScheduleForDestruction(m_Control);

			// Ensure that 'DestroyControl' is not called again for this control.
			m_Control.Release();
		}
	}
	EditorControlHandler* Editor::CreateControlHandler()
	{
		return new EditorControlHandler(this, GetControl());
	}

	void Editor::OnBeginEdit(Node& node, Column& column)
	{
		m_Node = &node;
		m_Column = &column;

		m_IsEditCanceled = false;
		m_IsEditFinished = false;
	}
	void Editor::OnEndEdit()
	{
		DestroyControl();
		m_Node = nullptr;
		m_Column = nullptr;
	}

	bool Editor::BeginEdit(Node& node, Column& column, const Rect& cellRect)
	{
		OnBeginEdit(node, column);

		// Before doing anything we send an event asking if editing of this item is really wanted.
		MainWindow& mainWindow = *GetMainWindow();
		if (mainWindow.SendEditingStartedEvent(node, this))
		{
			m_Control = CreateControl(mainWindow, cellRect, m_Node->GetCellValue(column));

			// There might be no editor control for the given item
			if (m_Control)
			{
				m_Control->PushEventHandler(CreateControlHandler());
				m_Control->SetFocus();

				return true;
			}
		}

		OnEndEdit();
		return false;
	}
	bool Editor::EndEdit()
	{
		if (m_Node && m_Column)
		{
			m_IsEditFinished = true;
			Utility::ScopeGuard atExit = [this]()
			{
				OnEndEdit();
			};

			if (m_Control)
			{
				// Try to get the value, normally we should succeed but if we fail, don't
				// return immediately, we still need to destroy the edit control.
				Any value = GetValue(*m_Control);
				DestroyControl();
				GetView()->SetFocus();

				MainWindow* mainWindow = GetMainWindow();
				if (!value.IsNull() && mainWindow->SendEditingDoneEvent(*m_Node, this, false, value))
				{
					if (m_Node->SetCellValue(*m_Column, std::move(value)))
					{
						mainWindow->OnCellChanged(*m_Node, m_Column);
					}
				}
			}
		}
		return false;
	}
	void Editor::CancelEdit()
	{
		m_IsEditCanceled = true;
		OnEndEdit();
	}

	Editor::Editor()
	{
	}
	Editor::~Editor()
	{
		DestroyControl();
	}

	View* Editor::GetView() const
	{
		return m_Column ? m_Column->GetView() : nullptr;
	}
	MainWindow* Editor::GetMainWindow() const
	{
		return m_Column ? m_Column->GetView()->GetMainWindow() : nullptr;
	}
}

namespace kxf::UI::DataView
{
	void EditorControlHandler::DoOnIdle(wxIdleEvent& event)
	{
		event.Skip();

		if (m_SetFocusOnIdle)
		{
			m_SetFocusOnIdle = false;
			if (wxWindow::FindFocus() != m_EditorCtrl)
			{
				m_EditorCtrl->SetFocus();
			}
		}
	}
	void EditorControlHandler::DoOnTextEnter(wxCommandEvent& event)
	{
		m_IsFinished = true;
		EndEdit();
	}
	void EditorControlHandler::DoOnChar(wxKeyEvent& event)
	{
		switch (event.GetKeyCode())
		{
			case WXK_ESCAPE:
			{
				m_IsFinished = true;
				CancelEdit();
				break;
			}
			case WXK_RETURN:
			{
				if (!event.HasAnyModifiers())
				{
					m_IsFinished = true;
					EndEdit();
					break;
				}

				// Ctrl/Alt/Shift-Enter is not handled specially
				[[fallthrough]];
			}
			default:
			{
				event.Skip();
			}
		};
	}
	void EditorControlHandler::DoOnKillFocus(wxFocusEvent& event)
	{
		event.Skip();

		if (!m_IsFinished)
		{
			m_IsFinished = true;
			EndEdit();
		}
	}
	void EditorControlHandler::DoOnMouse(wxMouseEvent& event)
	{
		const Point originalPos = Point(event.GetPosition());
		event.SetPosition(m_EditorCtrl->GetPosition());
		GetEditor()->GetMainWindow()->ProcessWindowEvent(event);
		event.SetPosition(originalPos);

		event.Skip();
	}

	EditorControlHandler::EditorControlHandler(Editor* editor, wxWindow* control)
		:m_Editor(editor), m_EditorCtrl(control)
	{
		m_IsFinished = false;
		m_SetFocusOnIdle = false;

		Bind(wxEVT_IDLE, &EditorControlHandler::OnIdle, this);
		Bind(wxEVT_CHAR, &EditorControlHandler::OnChar, this);
		Bind(wxEVT_KILL_FOCUS, &EditorControlHandler::OnKillFocus, this);
		Bind(wxEVT_TEXT_ENTER, &EditorControlHandler::OnTextEnter, this);

		// Causes drag events when focus in editor and such. Disabled for now.
		//Bind(wxEVT_MOTION, &EditorControlHandler::OnMouse, this);
	}
}
