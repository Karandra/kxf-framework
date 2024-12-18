#pragma once
#include "Common.h"
#include <wx/valnum.h>

namespace kxf::UI::Private
{
	KXF_API void GetValidatorControlValue(const wxValidator& validator, wxString& value, int& position, XChar& character);
}

namespace kxf::UI
{
	template<class T>
	class IntegerValidator: public wxIntegerValidator<T>
	{
		public:
			using Base = wxIntegerValidator<T>;

		protected:
			bool IsCharOk(const wxString& value, int position, XChar character) const override
			{
				UI::Private::GetValidatorControlValue(*this, const_cast<wxString&>(value), position, character);
				return Base::IsCharOk(value, position, character);
			}

		public:
			IntegerValidator(T* value = nullptr, int style = wxNUM_VAL_DEFAULT)
				:Base(value, style)
			{
			}

		public:
			IntegerValidator* Clone() const override
			{
				return new IntegerValidator(*this);
			}
	};

	template<class T>
	class FloatingPointValidator: public wxFloatingPointValidator<T>
	{
		public:
			using Base = wxFloatingPointValidator<T>;

		protected:
			bool IsCharOk(const wxString& value, int position, XChar character) const override
			{
				UI::Private::GetValidatorControlValue(*this, const_cast<wxString&>(value), position, character);
				return Base::IsCharOk(value, position, character);
			}

		public:
			FloatingPointValidator(T* value = nullptr, int style = wxNUM_VAL_DEFAULT)
				:Base(value, style)
			{
			}

		public:
			FloatingPointValidator* Clone() const override
			{
				return new FloatingPointValidator(*this);
			}
	};
}
