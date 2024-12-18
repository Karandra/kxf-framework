#pragma once
#include "StylesheetStorage.h"

namespace kxf::Sciter
{
	class KXF_API MasterStylesheetStorage final: public StylesheetStorage
	{
		public:
			static MasterStylesheetStorage& GetInstance() noexcept
			{
				static MasterStylesheetStorage instance;
				return instance;
			}

		private:
			MasterStylesheetStorage() noexcept = default;
			~MasterStylesheetStorage() = default;

		public:
			bool ApplyGlobally() const;
	};
}
