#pragma once
#include "Common.h"
#include "kxf/FileSystem/FSActionEvent.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/Crypto/SecretValue.h"

namespace kxf
{
	class KXF_API ArchiveEvent: public FSActionEvent
	{
		public:
			kxf_EVENT_MEMBER(ArchiveEvent, IdentifyFormat);

		private:
			FileItem m_FileItem;
			SecretValue m_Password;

		public:
			ArchiveEvent() = default;

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ArchiveEvent>(std::move(*this));
			}
			
			const FileItem& GetItem() const noexcept
			{
				return m_FileItem;
			}
			FileItem& GetItem() noexcept
			{
				return m_FileItem;
			}
			void SetItem(FileItem item) noexcept
			{
				m_FileItem = std::move(item);
			}

			size_t GetFileIndex() const noexcept
			{
				if (auto id = m_FileItem.GetUniqueID().ToLocallyUniqueID())
				{
					return static_cast<size_t>(id.ToInt());
				}
				return std::numeric_limits<size_t>::max();
			}
			void SetFileIndex(size_t value) noexcept
			{
				m_FileItem.SetUniqueID(LocallyUniqueID(value));
			}

			const SecretValue& GetPassword() const& noexcept
			{
				return m_Password;
			}
			SecretValue GetPassword() && noexcept
			{
				return std::move(m_Password);
			}
			void SetPassword(SecretValue password) noexcept
			{
				m_Password = std::move(password);
			}
	};
}
