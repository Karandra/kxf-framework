#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/String.h"
#include "kxf/Core/DataSize.h"
enum wxSeekMode: int;
enum wxStreamError: int;

namespace kxf
{
	enum class IOStreamAccess: uint32_t
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		ReadAttributes = 1 << 2,
		WriteAttributes = 1 << 3,

		RW = Read|Write,
		AllAccess = RW|ReadAttributes|WriteAttributes
	};
	kxf_FlagSet_Declare(IOStreamAccess);

	enum class IOStreamShare: uint32_t
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		Delete = 1 << 2,

		Everything = Read|Write|Delete
	};
	kxf_FlagSet_Declare(IOStreamShare);

	enum class IOStreamFlag: uint32_t
	{
		None = 0,

		Normal = 1 << 0,
		AllowDirectories = 1 << 1
	};
	kxf_FlagSet_Declare(IOStreamFlag);

	enum class IOStreamSeek
	{
		FromStart,
		FromCurrent,
		FromEnd,
	};
	enum class IOStreamDisposition
	{
		OpenExisting,
		OpenAlways,
		CreateNew,
		CreateAlways,
	};
}

namespace kxf::IO
{
	KXF_API std::optional<wxSeekMode> ToWxSeekMode(IOStreamSeek seekMode) noexcept;
	KXF_API std::optional<IOStreamSeek> FromWxSeekMode(wxSeekMode seekMode) noexcept;
}
