#pragma once
#include "kxf/Sciter/Common.h"
#include "kxf/Utility/Container.h"

namespace kxf::Sciter
{
	class Host;
}

namespace kxf::Sciter
{
	class KXF_API StylesheetStorage
	{
		private:
			std::vector<String> m_Items;

		public:
			StylesheetStorage() noexcept = default;
			virtual ~StylesheetStorage() = default;

		public:
			bool AddItem(String stylesheet);
			void ClearItems() noexcept;
			size_t GetItemCount() const noexcept
			{
				return m_Items.size();
			}
			size_t CopyItems(const StylesheetStorage& other);
			size_t TakeItems(StylesheetStorage&& other);

			Enumerator<const String&> EnumItems() const &;
			Enumerator<String> EnumItems() &&;

			bool Apply(Host& host, const FSPath& basePath = {}) const;
			bool Apply(Host& host, const URI& baseURI) const;
	};
}
