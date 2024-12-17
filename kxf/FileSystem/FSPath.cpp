#include "KxfPCH.h"
#include "FSPath.h"
#include "LegacyVolume.h"
#include "StorageVolume.h"
#include "kxf/System/HResult.h"
#include "kxf/System/NativeAPI.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/Utility/String.h"
#include "Private/NamespacePrefix.h"

#include <pathcch.h>
#include <locale>
#include <shlwapi.h>
#include "kxf/System/UndefWindows.h"
#pragma warning(disable: 4995) // 'PathCanonicalizeW': name was marked as #pragma deprecated

namespace
{
	constexpr size_t g_GUIDLength = 36;
	constexpr size_t g_VolumePathPrefixLength = 6;
	constexpr size_t g_VolumePathTotalLength = g_VolumePathPrefixLength + g_GUIDLength + 2;

	constexpr auto g_PathSeparator = kxS('\\');
}
namespace
{
	kxf::String ConcatWithNamespace(const kxf::String& path, kxf::FSPathNamespace ns)
	{
		using namespace kxf;

		if (ns != FSPathNamespace::None && !path.IsEmpty())
		{
			return FileSystem::GetNamespaceString(ns) + path;
		}
		return path;
	}
	kxf::String ExtractBefore(const kxf::String& path, kxf::XChar c, bool reverse = false)
	{
		using namespace kxf;

		const size_t pos = reverse ? path.ReverseFind(c) : path.Find(c);
		if (pos != String::npos)
		{
			String result = path.SubLeft(pos);
			if (!result.IsEmpty() && result.back() == g_PathSeparator)
			{
				result.RemoveRight(1);
			}
			return result;
		}
		return {};
	}
	kxf::String ExtractAfter(const kxf::String& path, kxf::XChar c, size_t count = kxf::String::npos, bool reverse = false)
	{
		using namespace kxf;

		const size_t pos = reverse ? path.ReverseFind(c) : path.Find(c);
		if (pos != String::npos && pos + 1 < path.length())
		{
			String result = path.SubMid(pos + 1, count);
			if (!result.IsEmpty() && result[0] == g_PathSeparator)
			{
				result.Remove(0, 1);
			}
			return result;
		}
		return {};
	}
}

namespace kxf
{
	FSPath FSPath::FromStringUnchecked(String string, FSPathNamespace ns)
	{
		FSPath path;
		path.m_Path = std::move(string);
		path.m_Namespace = ns;

		return path;
	}

	void FSPath::AssignFromPath(String path)
	{
		m_Path = std::move(path);
		if (!m_Path.IsEmpty())
		{
			// It's important to process namespace before normalization,
			// because normalization can remove some namespace symbols
			// and normalization assumes there is no namespace prefix anymore.
			ProcessNamespace();
			Normalize();
		}
	}
	void FSPath::ProcessNamespace()
	{
		// We need to remove any leading spaces before searching for a namespace.
		// This is duplicated in the 'Normalize' function but anyway we need it here.
		m_Path.TrimLeft();

		const size_t namespacePrefixLength = DetectNamespacePrefix(m_Path, m_Namespace);
		if (namespacePrefixLength != 0)
		{
			m_Path.Remove(0, namespacePrefixLength);
		}
	}
	void FSPath::Normalize()
	{
		m_Path.TrimLeft();

		bool removeNextSlash = false;
		for (size_t i = 0; i < m_Path.length(); i++)
		{
			auto& c = m_Path[i];

			// Replace forward slashes with backward slashes
			if (c == '/')
			{
				c = g_PathSeparator;
			}

			// Remove any duplicating slashes
			if (c == g_PathSeparator)
			{
				if (removeNextSlash || i + 1 == m_Path.length())
				{
					m_Path.Remove(i, 1);
					i--;
				}
				else
				{
					removeNextSlash = true;
				}
			}
			else
			{
				removeNextSlash = false;
			}
		}

		using namespace FileSystem::Private;
		if (m_Path.Contains(NamespacePrefix::DotRelative1) ||
			m_Path.Contains(NamespacePrefix::DotRelative5) ||
			m_Path.Contains(NamespacePrefix::DotRelative6))
		{
			SimplifyPath();
		}
	}

	bool FSPath::CheckIsLegacyVolume(const String& path) const
	{
		if (path.length() >= 2 && path[1] == ':')
		{
			return LegacyVolume::FromChar(path[0]).IsValid();
		}
		return false;
	}
	bool FSPath::CheckIsVolumeGUID(const String& path) const
	{
		// Format: '\\?\Volume{66843779-55ae-45c5-9abe-b67ccee14079}\', but we're store path without namespace and trailing separators
		// so it'll be this instead: 'Volume{66843779-55ae-45c5-9abe-b67ccee14079}'.
		if (path.length() >= g_VolumePathTotalLength)
		{
			const bool prefixCorrect = std::char_traits<XChar>::compare(path.wc_str(), L"Volume", g_VolumePathPrefixLength) == 0;
			const bool bracesCorrect = path[g_VolumePathPrefixLength] == '{' && path[g_VolumePathTotalLength - 1] == '}';
			return prefixCorrect && bracesCorrect;
		}
		return false;
	}
	size_t FSPath::DetectNamespacePrefix(const String& path, kxf::FSPathNamespace& ns) const
	{
		using namespace FileSystem::Private;

		// All namespaces starts from at least one '\'
		if (path.IsEmpty() || path[0] != g_PathSeparator)
		{
			ns = FSPathNamespace::None;

			// Check for non-namespace prefixes
			if (path.StartsWith(NamespacePrefix::DotRelative1) || path.StartsWith(NamespacePrefix::DotRelative3))
			{
				return std::size(NamespacePrefix::DotRelative1) - 1;
			}
			else if (path.StartsWith(NamespacePrefix::DotRelative2) || path.StartsWith(NamespacePrefix::DotRelative4))
			{
				return std::size(NamespacePrefix::DotRelative2) - 1;
			}
			return 0;
		}

		// Test for every namespace starting from the longest prefix
		// Length: 8
		if (path.StartsWith(NamespacePrefix::Win32FileUNC))
		{
			ns = FSPathNamespace::Win32FileUNC;
			return std::size(NamespacePrefix::Win32FileUNC) - 1;
		}
		else if (path.StartsWith(NamespacePrefix::NetworkUNC))
		{
			ns = FSPathNamespace::NetworkUNC;
			return std::size(NamespacePrefix::NetworkUNC) - 1;
		}

		// Length: 4
		if (path.StartsWith(NamespacePrefix::Win32File))
		{
			ns = FSPathNamespace::Win32File;
			return std::size(NamespacePrefix::Win32File) - 1;
		}
		else if (path.StartsWith(NamespacePrefix::Win32Device))
		{
			ns = FSPathNamespace::Win32Device;
			return std::size(NamespacePrefix::Win32Device) - 1;
		}

		// Length: 2
		if (path.StartsWith(NamespacePrefix::Network))
		{
			ns = FSPathNamespace::Network;
			return std::size(NamespacePrefix::Network) - 1;
		}

		// Length: 1
		if (path.StartsWith(NamespacePrefix::NT))
		{
			ns = FSPathNamespace::NT;
			return std::size(NamespacePrefix::NT) - 1;
		}

		return 0;
	}

	bool FSPath::CheckStringOnAssignPath(const String& path) const
	{
		FSPathNamespace ns = FSPathNamespace::None;
		return !CheckIsLegacyVolume(path) && !CheckIsVolumeGUID(path) && DetectNamespacePrefix(path, ns) == 0;
	}
	bool FSPath::CheckStringOnAssignName(const String& name) const
	{
		return CheckStringOnAssignPath(name);
	}

	String FSPath::CreateFullPath(FSPathNamespace ns, FlagSet<FSPathFormat> format) const
	{
		if (!m_Path.IsEmpty())
		{
			String result = ns != FSPathNamespace::None ? ConcatWithNamespace(m_Path, ns) : m_Path;

			if (format & FSPathFormat::TrailingSeparator)
			{
				result += g_PathSeparator;
			}
			return result;
		}
		return {};
	}

	bool FSPath::IsNull() const
	{
		return m_Path.IsEmpty();
	}
	bool FSPath::IsSameAs(const FSPath& other, bool caseSensitive) const
	{
		return m_Namespace == other.m_Namespace && m_Path.IsSameAs(other.m_Path, caseSensitive ? StringActionFlag::None : StringActionFlag::IgnoreCase);
	}
	bool FSPath::IsAbsolute() const
	{
		// Path is absolute if it has a namespace or starts with a volume (a disk designator)
		return !IsNull() && (m_Namespace != FSPathNamespace::None || HasAnyVolume());
	}
	bool FSPath::IsRelative() const
	{
		return !IsNull() && !IsAbsolute();
	}
	bool FSPath::IsUNCPath() const
	{
		return m_Namespace == FSPathNamespace::Win32FileUNC || m_Namespace == FSPathNamespace::NetworkUNC;
	}

	bool FSPath::ContainsPath(const FSPath& path, bool caseSensitive) const
	{
		return m_Path.Contains(path.GetFullPath(), caseSensitive ? StringActionFlag::None : StringActionFlag::IgnoreCase);
	}
	size_t FSPath::GetComponentCount() const
	{
		size_t count = 0;
		for (XChar c: m_Path)
		{
			if (c == g_PathSeparator)
			{
				count++;
			}
		}

		if (HasAnyVolume())
		{
			count++;
		}
		return count;
	}
	std::vector<StringView> FSPath::EnumComponents() const
	{
		std::vector<StringView> parts;
		m_Path.SplitBySeparator(g_PathSeparator, [&](StringView view)
		{
			if (!view.empty())
			{
				parts.emplace_back(view);
			}
			return true;
		});

		return parts;
	}

	String FSPath::GetFullPath(FlagSet<FSPathFormat> format) const
	{
		return CreateFullPath(FSPathNamespace::None, format);
	}
	String FSPath::GetFullPathTryNS(FSPathNamespace ns, FlagSet<FSPathFormat> format) const
	{
		if (IsAbsolute())
		{
			if (m_Namespace != FSPathNamespace::None)
			{
				return CreateFullPath(m_Namespace, format);
			}
			else if (ns != FSPathNamespace::None && ns != FSPathNamespace::Any)
			{
				return CreateFullPath(ns, format);
			}
		}
		return CreateFullPath(FSPathNamespace::None, format);
	}
	String FSPath::GetFullPathRequireNS(FSPathNamespace ns, FlagSet<FSPathFormat> format) const
	{
		if (ns != FSPathNamespace::None && IsAbsolute())
		{
			if (ns == FSPathNamespace::Any)
			{
				if (m_Namespace != FSPathNamespace::None)
				{
					return CreateFullPath(m_Namespace, format);
				}
				return {};
			}
			else
			{
				return CreateFullPath(ns, format);
			}
		}
		return {};
	}
	String FSPath::GetDisplayString() const
	{
		if (!m_Path.IsEmpty())
		{
			FlagSet<FSPathFormat> format;
			format.Add(FSPathFormat::TrailingSeparator, m_Path.GetLength() == 2 && CheckIsLegacyVolume(m_Path));

			return CreateFullPath(m_Namespace != FSPathNamespace::None ? m_Namespace : FSPathNamespace::None, format);
		}
		return {};
	}

	bool FSPath::HasVolume() const
	{
		return CheckIsVolumeGUID(m_Path);
	}
	bool FSPath::HasLegacyVolume() const
	{
		return CheckIsLegacyVolume(m_Path);
	}
	StorageVolume FSPath::GetVolume() const
	{
		// StorageVolume constructor does the validity check and extracts volume path
		return *this;
	}
	StorageVolume FSPath::GetAsVolume() const
	{
		StorageVolume volume = GetVolume();
		if (!volume)
		{
			volume = GetLegacyVolume();
		}
		return volume;
	}
	LegacyVolume FSPath::GetLegacyVolume() const
	{
		if (HasLegacyVolume())
		{
			return LegacyVolume::FromChar(m_Path[0]);
		}
		return {};
	}
	LegacyVolume FSPath::GetAsLegacyVolume() const
	{
		LegacyVolume legacyVolume = GetLegacyVolume();
		if (!legacyVolume)
		{
			legacyVolume = GetVolume().GetLegacyVolume();
		}
		return legacyVolume;
	}
	FSPath& FSPath::SetVolume(const LegacyVolume& drive)
	{
		if (HasLegacyVolume())
		{
			if (drive)
			{
				// Replace the disk designator
				m_Path[0] = drive.GetChar();
			}
			else
			{
				// Remove the disk designator
				m_Path.Remove(0, 2);
				Normalize();
			}
		}
		else if (HasVolume())
		{
			if (drive)
			{
				// Replace with legacy drive path
				char disk[] = "\0:\\";
				disk[0] = drive.GetChar();
				m_Path.ReplaceRange(0, g_VolumePathTotalLength, StringViewOf(disk));
			}
			else
			{
				// Remove the volume
				m_Path.Remove(0, g_VolumePathTotalLength);
			}
			Normalize();
		}
		else
		{
			// Perpend a new disk designator
			char disk[] = "\0:\\";
			disk[0] = drive.GetChar();
			m_Path.Prepend(disk);

			Normalize();
		}
		return *this;
	}
	FSPath& FSPath::SetVolume(const StorageVolume& volume)
	{
		if (HasLegacyVolume())
		{
			if (volume)
			{
				// Replace with volume path
				String path = volume.GetPath().GetFullPath(FSPathFormat::TrailingSeparator);
				m_Path.ReplaceRange(0, 2, path);
			}
			else
			{
				// Remove the disk designator
				m_Path.Remove(0, 2);
			}
			Normalize();
		}
		else if (HasVolume())
		{
			if (volume)
			{
				// Replace with a new volume path
				String path = volume.GetPath().GetFullPath(FSPathFormat::TrailingSeparator);
				m_Path.ReplaceRange(0, g_VolumePathTotalLength, path);
			}
			else
			{
				// Remove the volume
				m_Path.Remove(0, g_VolumePathTotalLength);
			}
			Normalize();
		}
		else
		{
			// Prepend a new volume path
			String path = volume.GetPath().GetFullPath(FSPathFormat::TrailingSeparator);
			m_Path.Prepend(std::move(path));

			Normalize();
		}
		return *this;
	}

	String FSPath::GetPath() const
	{
		if (HasLegacyVolume())
		{
			// Return after the disk designator
			return m_Path.SubMid(2);
		}
		else if (HasVolume())
		{
			// Return after GUID path
			return m_Path.SubMid(g_VolumePathTotalLength);
		}
		else
		{
			// Return the path itself
			return m_Path;
		}
	}
	FSPath& FSPath::SetPath(String path)
	{
		if (CheckStringOnAssignPath(path))
		{
			if (HasLegacyVolume())
			{
				// Replace after the disk designator
				m_Path.Remove(2, String::npos);
				m_Path += g_PathSeparator;
				m_Path += path;
			}
			else if (HasVolume())
			{
				// Replace after GUID path
				m_Path.Truncate(g_VolumePathTotalLength);
				m_Path += g_PathSeparator;
				m_Path += path;
			}
			else
			{
				// Replace the full path
				m_Path = std::move(path);
			}
			Normalize();
		}
		return *this;
	}
	FSPath& FSPath::SimplifyPath()
	{
		if (!IsNull())
		{
			const FSPathNamespace ns = m_Namespace;

			bool isSuccess = false;
			if (NativeAPI::KernelBase::PathCchCanonicalizeEx)
			{
				String result;
				constexpr size_t length = std::numeric_limits<int16_t>::max();
				if (HResult(NativeAPI::KernelBase::PathCchCanonicalizeEx(Utility::StringBuffer(result, length, true), length, m_Path.wc_str(), PATHCCH_ALLOW_LONG_PATHS|PATHCCH_FORCE_ENABLE_LONG_NAME_PROCESS)))
				{
					m_Path = std::move(result);
					ProcessNamespace();

					isSuccess = true;
				}
			}
			else if (GetLength() < MAX_PATH && NativeAPI::ShlWAPI::PathCanonicalizeW)
			{
				wchar_t result[MAX_PATH] = {};
				if (NativeAPI::ShlWAPI::PathCanonicalizeW(result, m_Path.wc_str()))
				{
					m_Path = std::move(result);
					ProcessNamespace();

					isSuccess = true;
				}
			}

			EnsureNamespaceSet(ns);
			if (!isSuccess)
			{
				*this = {};
			}
		}
		return *this;
	}

	String FSPath::GetName() const
	{
		// Return everything after last path delimiter or itself
		String path = ExtractAfter(m_Path, g_PathSeparator, String::npos, true);
		return path.IsEmpty() ? m_Path : path;
	}
	FSPath& FSPath::SetName(const String& name)
	{
		if (CheckStringOnAssignName(name))
		{
			const size_t pos = m_Path.ReverseFind(g_PathSeparator);
			if (pos != String::npos)
			{
				const size_t dot = m_Path.Find('.', {}, pos);
				if (dot != String::npos)
				{
					m_Path.ReplaceRange(pos + 1, dot - pos, name);
				}
				else
				{
					m_Path.ReplaceRange(pos + 1, m_Path.length() - pos, name);
				}
				Normalize();
			}
		}
		return *this;
	}

	String FSPath::GetExtension() const
	{
		// Return extension without a dot
		return ExtractAfter(m_Path, '.', String::npos, true);
	}
	FSPath& FSPath::SetExtension(const String& ext)
	{
		auto Replace = [this](const String& ext)
		{
			const size_t pos = m_Path.ReverseFind('.');
			if (pos != String::npos)
			{
				m_Path.ReplaceRange(pos + 1, m_Path.length() - pos, ext);
			}
			else
			{
				m_Path += '.';
				m_Path += ext;
			}
		};

		if (String extWithoutDot; ext.StartsWith('.', &extWithoutDot))
		{
			Replace(extWithoutDot);
		}
		else
		{
			Replace(ext);
		}
		Normalize();

		return *this;
	}

	FSPath FSPath::GetAfter(const FSPath& start) const
	{
		// this: C:\Program Files (x86)\Common Files\Microsoft
		// start: C:\Program Files (x86)
		// return: Common Files\Microsoft

		String fullPath = GetFullPath();
		if (fullPath.SubLeft(start.GetLength()).IsSameAs(start.m_Path, StringActionFlag::IgnoreCase))
		{
			fullPath = fullPath.Remove(0, start.GetLength());
		}
		return FSPath(std::move(fullPath)).EnsureNamespaceSet(m_Namespace);
	}
	FSPath FSPath::GetBefore(const FSPath& end) const
	{
		// this: C:\Program Files (x86)\Common Files\Microsoft
		// end: Common Files\Microsoft
		// return: C:\Program Files (x86)

		String fullPath = GetFullPath();
		if (fullPath.SubRight(end.GetLength()).IsSameAs(end.m_Path, StringActionFlag::IgnoreCase))
		{
			fullPath = fullPath.Remove(fullPath.length() - end.GetLength(), end.GetLength());
		}
		return FSPath(std::move(fullPath)).EnsureNamespaceSet(m_Namespace);
	}
	FSPath FSPath::GetParent() const
	{
		return FSPath(ExtractBefore(m_Path, g_PathSeparator, true)).EnsureNamespaceSet(m_Namespace);
	}
	FSPath& FSPath::RemoveLastPart()
	{
		*this = GetParent();
		return *this;
	}
	
	FSPath& FSPath::Append(const FSPath& other)
	{
		if (IsNull() || other.IsRelative())
		{
			if (!m_Path.IsEmpty())
			{
				m_Path += g_PathSeparator;
			}
			m_Path += other.m_Path;

			Normalize();
		}
		return *this;
	}
	FSPath& FSPath::Concat(const FSPath& other)
	{
		if (IsNull() || other.IsRelative())
		{
			m_Path += other.m_Path;
			Normalize();
		}
		return *this;
	}
}

namespace kxf
{
	uint64_t BinarySerializer<FSPath>::Serialize(IOutputStream& stream, const FSPath& value) const
	{
		return Serialization::WriteObject(stream, value.m_Path) + Serialization::WriteObject(stream, value.m_Namespace);
	}
	uint64_t BinarySerializer<FSPath>::Deserialize(IInputStream& stream, FSPath& value) const
	{
		return Serialization::ReadObject(stream, value.m_Path) + Serialization::ReadObject(stream, value.m_Namespace);
	}
}
