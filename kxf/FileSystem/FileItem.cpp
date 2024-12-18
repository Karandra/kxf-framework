#include "kxf-pch.h"
#include "FSPath.h"
#include "FileItem.h"
#include "IFileSystem.h"

namespace kxf
{
	FileItem& FileItem::Refresh(const IFileSystem& fileSystem)
	{
		*this = fileSystem.GetItem(m_Path);
		return *this;
	}
}

namespace kxf
{
	uint64_t BinarySerializer<FileItem>::Serialize(IOutputStream& stream, const FileItem& value) const
	{
		return Serialization::WriteObject(stream, value.m_Path) +
			Serialization::WriteObject(stream, value.m_Size) +
			Serialization::WriteObject(stream, value.m_CompressedSize) +
			Serialization::WriteObject(stream, value.m_CreationTime) +
			Serialization::WriteObject(stream, value.m_LastAccessTime) +
			Serialization::WriteObject(stream, value.m_ModificationTime) +
			Serialization::WriteObject(stream, value.m_UniqueID) +
			Serialization::WriteObject(stream, value.m_Attributes) +
			Serialization::WriteObject(stream, value.m_ReparsePointTags);
	}
	uint64_t BinarySerializer<FileItem>::Deserialize(IInputStream& stream, FileItem& value) const
	{
		return Serialization::ReadObject(stream, value.m_Path) +
			Serialization::ReadObject(stream, value.m_Size) +
			Serialization::ReadObject(stream, value.m_CompressedSize) +
			Serialization::ReadObject(stream, value.m_CreationTime) +
			Serialization::ReadObject(stream, value.m_LastAccessTime) +
			Serialization::ReadObject(stream, value.m_ModificationTime) +
			Serialization::ReadObject(stream, value.m_UniqueID) +
			Serialization::ReadObject(stream, value.m_Attributes) +
			Serialization::ReadObject(stream, value.m_ReparsePointTags);
	}
}
