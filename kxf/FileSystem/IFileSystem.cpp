#include "kxf-pch.h"
#include "IFileSystem.h"
#include "NullFileSystem.h"
#include "kxf/IO/IStream.h"

namespace
{
	using namespace kxf;

	kxf::FileSystem::NullFileSystem& GetNullFS() noexcept
	{
		static kxf::FileSystem::NullFileSystem instance;
		return instance;
	}

	template<class TStream, class TFileSystem, class TID>
	std::shared_ptr<TStream> QueryStream(TFileSystem& fs, const TID& pathOrUniqueID, FlagSet<IOStreamAccess> access, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<FSActionFlag> actionFlags)
	{
		if (auto stream = fs.CreateStream(pathOrUniqueID, access, disposition, share, IOStreamFlag::None, actionFlags))
		{
			return stream->QueryInterface<TStream>();
		}
		return nullptr;
	}
}

namespace kxf
{
	std::shared_ptr<IInputStream> IFileSystem::OpenToRead(const FSPath& path, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<FSActionFlag> flags) const
	{
		return QueryStream<IInputStream>(const_cast<IFileSystem&>(*this), path, IOStreamAccess::Read, disposition, share, flags);
	}
	std::shared_ptr<IOutputStream> IFileSystem::OpenToWrite(const FSPath& path, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<FSActionFlag> flags)
	{
		return QueryStream<IOutputStream>(*this, path, IOStreamAccess::Write, disposition, share, flags);
	}
}

namespace kxf
{
	std::shared_ptr<IInputStream> IFileSystemWithID::OpenToRead(const UniversallyUniqueID& id, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<FSActionFlag> flags) const
	{
		return QueryStream<IInputStream>(const_cast<IFileSystemWithID&>(*this), id, IOStreamAccess::Read, disposition, share, flags);
	}
	std::shared_ptr<IOutputStream> IFileSystemWithID::OpenToWrite(const UniversallyUniqueID& id, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<FSActionFlag> flags)
	{
		return QueryStream<IOutputStream>(*this, id, IOStreamAccess::Write, disposition, share, flags);
	}
}

namespace kxf::FileSystem
{
	IFileSystem& GetNullFileSystem() noexcept
	{
		return GetNullFS();
	}
	IFileSystemWithID& GetNullFileSystemWithID() noexcept
	{
		return GetNullFS();
	}
}
