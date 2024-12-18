#pragma once
#include "Common.h"

namespace kxf
{
	class ITextWidget;
}

namespace kxf::DataView
{
	class KXF_API TextBox: public IDataViewCellEditor
	{
		private:
			std::shared_ptr<ITextWidget> m_Widget;

		protected:
			// IDataViewCellEditor
			std::shared_ptr<IWidget> CreateWidget(std::shared_ptr<IWidget> parent, const EditorInfo& editorInfo) override;

		public:
			TextBox() = default;

		public:
			// IDataViewCellEditor
			bool BeginEdit(const EditorInfo& editorInfo, Any value) override;
			Any GetValue(const EditorInfo& editorInfo) const override;
	};
}
