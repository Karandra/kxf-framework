#pragma once
#include "Common.h"
#include "BasicWindowWidget.h"
#include "../IDalogWidget.h"
class wxDialog;

namespace kxf::Private
{
	class KXF_API BasicDialogWidgetBase: public BasicWindowWidgetBase
	{
		public:
			BasicDialogWidgetBase(IWidget& widget) noexcept
				:BasicWindowWidgetBase(widget)
			{
			}
			~BasicDialogWidgetBase() = default;

		public:
			// BasicWxWidgetBase
			wxDialog* GetWxWindow() const noexcept;

		public:
			// BasicDialogWidgetBase
			bool IsModal() const;
			void EndModal(WidgetID retCode);
			WidgetID ShowModal();
			WidgetID GetReturnCode() const;

			WidgetID GetAffirmativeID() const;
			void SetAffirmativeID(WidgetID id);

			WidgetID GetEscapeID() const;
			void SetEscapeID(WidgetID id);
	};
}

namespace kxf::Private
{
	template<class TDerived, class TWindow, class TInterface, class TWindowImpl = BasicDialogWidgetBase>
	class BasicDialogWidget: public BasicWindowWidget<TDerived, TWindow, TInterface, TWindowImpl>
	{
		protected:
			using BasicWindowWidget<TDerived, TWindow, TInterface, TWindowImpl>::GetImpl;

		public:
			bool IsModal() const override
			{
				return GetImpl().IsModal();
			}
			void EndModal(WidgetID retCode) override
			{
				GetImpl().EndModal(retCode);
			}
			WidgetID ShowModal() override
			{
				return GetImpl().ShowModal();
			}
			WidgetID GetReturnCode() const override
			{
				return GetImpl().GetReturnCode();
			}

			WidgetID GetAffirmativeID() const override
			{
				return GetImpl().GetAffirmativeID();
			}
			void SetAffirmativeID(WidgetID id) override
			{
				GetImpl().SetAffirmativeID(id);
			}

			WidgetID GetEscapeID() const override
			{
				return GetImpl().GetEscapeID();
			}
			void SetEscapeID(WidgetID id) override
			{
				GetImpl().SetEscapeID(id);
			}
	};
}
