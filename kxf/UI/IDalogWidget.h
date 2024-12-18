#pragma once
#include "Common.h"
#include "ITopLevelWidget.h"

namespace kxf
{
	class KXF_API IDalogWidget: public RTTI::ExtendInterface<IDalogWidget, ITopLevelWidget>
	{
		kxf_RTTI_DeclareIID(IDalogWidget, {0xa6bd3c33, 0x4d1e, 0x4a13, {0x8a, 0x16, 0xb6, 0xc9, 0x26, 0x9e, 0xb0, 0xc1}});

		public:
			virtual ~IDalogWidget() = default;

		public:
			virtual bool IsModal() const = 0;
			virtual void EndModal(WidgetID retCode) = 0;
			virtual WidgetID ShowModal() = 0;
			virtual WidgetID GetReturnCode() const = 0;

			virtual WidgetID GetAffirmativeID() const = 0;
			virtual void SetAffirmativeID(WidgetID id) = 0;

			virtual WidgetID GetEscapeID() const = 0;
			virtual void SetEscapeID(WidgetID id) = 0;
	};
}
