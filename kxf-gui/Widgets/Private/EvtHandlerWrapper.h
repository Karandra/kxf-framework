#pragma once
#include "Common.h"
#include <wx/event.h>

namespace kxf
{
	class IWidget;
}

namespace kxf::WXUI::Private
{
	class KXF_API EvtHandlerWrapperBase final
	{
		private:
			IWidget& m_Widget;

		public:
			EvtHandlerWrapperBase(IWidget& widget) noexcept
				:m_Widget(widget)
			{
			}

		public:
			// wxEvtHandler
			bool TryBefore(wxEvent& anyEvent);
			bool TryAfter(wxEvent& anyEvent);

			bool OnDynamicBind(wxDynamicEventTableEntry& eventItem);

		public:
			// EvtHandlerWrapperBase
			bool TranslateTextEvent(IWidget& widget, wxEvent& event);
			bool TranslateScrollEvent(IWidget& widget, wxEvent& event);
			bool TranslateBookEvent(IWidget& widget, wxEvent& event);
	};
}

namespace kxf::WXUI
{
	template<class TDerived, class TBase>
	class EvtHandlerWrapper: public TBase
	{
		private:
			Private::EvtHandlerWrapperBase m_Wrapper;

		private:
			TDerived& Self() noexcept
			{
				return static_cast<TDerived&>(*this);
			}
			const TDerived& Self() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

		private:
			bool DoTryBefore(wxEvent& event) noexcept
			{
				return false;
			}
			bool DoTryAfter(wxEvent& event) noexcept
			{
				return false;
			}

		protected:
			// wxEvtHandler
			bool TryBefore(wxEvent& event) override
			{
				if (Self().DoTryBefore(event) || m_Wrapper.TryBefore(event))
				{
					return true;
				}
				return TBase::TryBefore(event);
			}
			bool TryAfter(wxEvent& event) override
			{
				if (TBase::TryAfter(event))
				{
					return true;
				}
				return Self().DoTryAfter(event) || m_Wrapper.TryAfter(event);
			}

			bool OnDynamicBind(wxDynamicEventTableEntry& eventItem) override
			{
				return m_Wrapper.OnDynamicBind(eventItem) && TBase::OnDynamicBind(eventItem);
			}

			// EvtHandlerWrapper
			bool TranslateTextEvent(IWidget& widget, wxEvent& event)
			{
				return m_Wrapper.TranslateTextEvent(widget, event);
			}
			bool TranslateScrollEvent(IWidget& widget, wxEvent& event)
			{
				return m_Wrapper.TranslateScrollEvent(widget, event);
			}
			bool TranslateBookEvent(IWidget& widget, wxEvent& event)
			{
				return m_Wrapper.TranslateBookEvent(widget, event);
			}

		public:
			EvtHandlerWrapper(IWidget& widget) noexcept
				:m_Wrapper(widget)
			{
			}
	};
}

namespace kxf::WXUI
{
	class SimpleEvtHandlerWrapper final: public EvtHandlerWrapper<SimpleEvtHandlerWrapper, wxEvtHandler>
	{
		friend class EvtHandlerWrapper;

		public:
			using EvtHandlerWrapper::EvtHandlerWrapper;
	};
}
