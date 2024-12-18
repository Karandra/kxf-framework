#pragma once
#include "../Common.h"
#include <wx/log.h>

namespace kxf::Log
{
	class KXF_API WxOverride final: public wxLog
	{
		public:
			static void Install();

		protected:
			void DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info) override;

		public:
			WxOverride() = default;

		public:
			void Flush() override;
	};
}
