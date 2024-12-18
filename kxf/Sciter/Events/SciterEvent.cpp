#include "kxf-pch.h"
#include "SciterEvent.h"
#include "kxf/Sciter/Host.h"

namespace kxf
{
	Sciter::Element SciterEvent::RetrieveElement(Sciter::ElementUID* uid) const
	{
		return m_Host->GetElementByUID(uid);
	}
	void SciterEvent::AcceptElement(Sciter::ElementUID*& uid, const Sciter::Element& element)
	{
		uid = element.GetUID();
	}
}
