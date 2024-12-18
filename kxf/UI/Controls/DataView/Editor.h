#pragma once
#include "Common.h"
#include "Renderer.h"
#include <wx/window.h>
#include <wx/weakref.h>

namespace kxf::UI::DataView
{
	class Node;
	class View;
	class Column;
	class Renderer;
	class MainWindow;
	class EditorControlHandler;
}

namespace kxf::UI::DataView
{
	class KXF_API Editor
	{
		friend class MainWindow;
		friend class EditorControlHandler;

		private:
			wxWindowRef m_Control;
			std::unique_ptr<wxValidator> m_Validator;
			Column* m_Column = nullptr;
			Node* m_Node = nullptr;
			bool m_IsEditable = true;

			bool m_IsEditCanceled = false;
			bool m_IsEditFinished = false;

		private:
			void OnBeginEdit(Node& node, Column& column);
			void OnEndEdit();

		protected:
			virtual void DestroyControl();
			virtual EditorControlHandler* CreateControlHandler();

			virtual bool BeginEdit(Node& node, Column& column, const Rect& cellRect);
			virtual bool EndEdit();
			virtual void CancelEdit();

		protected:
			virtual wxWindow* CreateControl(wxWindow& parent, const Rect& cellRect, Any value) = 0;
			virtual Any GetValue(wxWindow& control) const = 0;

			bool IsEditCanceled() const
			{
				return m_IsEditCanceled;
			}
			bool IsEditFinished() const
			{
				return m_IsEditFinished;
			}

		public:
			Editor();
			virtual ~Editor();

		public:
			wxWindow* GetControl() const
			{
				return m_Control ? m_Control.get() : nullptr;
			}
			Node* GetNode() const
			{
				return m_Node;
			}
			Column* GetColumn() const
			{
				return m_Column;
			}
			View* GetView() const;
			MainWindow* GetMainWindow() const;

			bool IsEditing() const
			{
				return m_Column != nullptr && m_Node != nullptr;
			}
			bool IsEditable() const
			{
				return m_IsEditable;
			}
			void SetEditable(bool isEditable = true)
			{
				m_IsEditable = isEditable;
			}

			bool HasValidator() const
			{
				return m_Validator != nullptr;
			}
			const wxValidator& GetValidator() const
			{
				return HasValidator() ? *m_Validator : wxDefaultValidator;
			}
			wxValidator& GetValidatorRef()
			{
				return *m_Validator;
			}

			void SetValidator(std::unique_ptr<wxValidator> validator)
			{
				m_Validator = std::move(validator);
			}
			template<class T, class... Args> T& NewValidator(Args&&... arg)
			{
				m_Validator = std::make_unique<T>(std::forward<Args>(arg)...);
				return static_cast<T&>(*m_Validator);
			}
	};
}


namespace kxf::UI::DataView
{
	class KXF_API EditorControlHandler: public wxEvtHandler
	{
		private:
			Editor* m_Editor = nullptr;
			wxWindow* m_EditorCtrl = nullptr;

			bool m_IsFinished = false;
			bool m_SetFocusOnIdle = false;

		private:
			void OnChar(wxKeyEvent& event)
			{
				DoOnChar(event);
			}
			void OnTextEnter(wxCommandEvent& event)
			{
				DoOnTextEnter(event);
			}
			void OnKillFocus(wxFocusEvent& event)
			{
				DoOnKillFocus(event);
			}
			void OnIdle(wxIdleEvent& event)
			{
				DoOnIdle(event);
			}
			void OnMouse(wxMouseEvent& event)
			{
				DoOnMouse(event);
			}

		protected:
			virtual void DoOnChar(wxKeyEvent& event);
			virtual void DoOnTextEnter(wxCommandEvent& event);
			virtual void DoOnKillFocus(wxFocusEvent& event);
			virtual void DoOnIdle(wxIdleEvent& event);
			virtual void DoOnMouse(wxMouseEvent& event);

			Editor* GetEditor() const
			{
				return m_Editor;
			}
			wxWindow* GetEditorControl() const
			{
				return m_EditorCtrl;
			}

		public:
			EditorControlHandler(Editor* editor, wxWindow* control);

		public:
			bool IsFinished() const
			{
				return m_IsFinished;
			}
			void SetFinished(bool finished)
			{
				m_IsFinished = finished;
			}

			bool ShouldSetFocusOnIdle() const
			{
				return m_SetFocusOnIdle;
			}
			void SetFocusOnIdle(bool focus = true)
			{
				m_SetFocusOnIdle = focus;
			}

			bool BeginEdit(Node& item, Column& column, const Rect& cellRect)
			{
				return m_Editor->BeginEdit(item, column, cellRect);
			}
			bool EndEdit()
			{
				return m_Editor->EndEdit();
			}
			void CancelEdit()
			{
				m_Editor->CancelEdit();
			}
	};
}
