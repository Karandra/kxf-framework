#include "kxf-pch.h"
#include "BasicDialogWidget.h"
#include <wx/dialog.h>

namespace kxf::Private
{
	// BasicWxWidgetBase
	wxDialog* BasicDialogWidgetBase::GetWxWindow() const noexcept
	{
		return static_cast<wxDialog*>(BasicWindowWidgetBase::GetWxWindow());
	}

	// BasicDialogWidgetBase
	bool BasicDialogWidgetBase::IsModal() const
	{
		return GetWxWindow()->IsModal();
	}
	void BasicDialogWidgetBase::EndModal(WidgetID retCode)
	{
		GetWxWindow()->EndModal(retCode.ToInt());
	}
	WidgetID BasicDialogWidgetBase::ShowModal()
	{
		return GetWxWindow()->ShowModal();
	}
	WidgetID BasicDialogWidgetBase::GetReturnCode() const
	{
		return GetWxWindow()->GetReturnCode();
	}

	WidgetID BasicDialogWidgetBase::GetAffirmativeID() const
	{
		return GetWxWindow()->GetAffirmativeId();
	}
	void BasicDialogWidgetBase::SetAffirmativeID(WidgetID id)
	{
		GetWxWindow()->SetAffirmativeId(id.ToInt());
	}

	WidgetID BasicDialogWidgetBase::GetEscapeID() const
	{
		return GetWxWindow()->GetEscapeId();
	}
	void BasicDialogWidgetBase::SetEscapeID(WidgetID id)
	{
		GetWxWindow()->SetEscapeId(id.ToInt());
	}
}
