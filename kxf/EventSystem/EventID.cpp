#include "kxf-pch.h"
#include "EventID.h"
#include <wx/event.h>

namespace
{
	const int64_t g_LastWxEventID = ::wxNewEventType();
	std::atomic<int64_t> g_SimpleEventID = 0;
}

namespace kxf
{
	size_t EventID::GetHash() const noexcept
	{
		if (auto value = std::get_if<int64_t>(&m_ID))
		{
			if (*value != 0)
			{
				return std::hash<int64_t>()(*value);
			}
		}
		else if (auto value = std::get_if<UniversallyUniqueID>(&m_ID))
		{
			if (!value->IsNull())
			{
				return std::hash<UniversallyUniqueID>()(*value);
			}
		}
		else if (auto value = std::get_if<String>(&m_ID))
		{
			if (!value->IsEmpty())
			{
				return std::hash<String>()(*value);
			}
		}
		return 0;
	}
	uint64_t EventID::Serialize(IOutputStream& stream) const
	{
		auto WriteIndex = [&]()
		{
			return Serialization::WriteObject(stream, static_cast<uint64_t>(m_ID.index()));
		};

		// We don't serialize type info

		uint64_t written = 0;
		if (auto value = std::get_if<int64_t>(&m_ID))
		{
			written += WriteIndex();
			written += Serialization::WriteObject(stream, *value);
		}
		else if (auto value = std::get_if<UniversallyUniqueID>(&m_ID))
		{
			written += WriteIndex();
			written += Serialization::WriteObject(stream, *value);
		}
		else if (auto value = std::get_if<String>(&m_ID))
		{
			written += WriteIndex();
			written += Serialization::WriteObject(stream, *value);
		}
		else
		{
			// Shouldn't happen, but just in case serialize as if it's an empty object
			written += Serialization::WriteObject(stream, static_cast<uint64_t>(0));
			written += Serialization::WriteObject(stream, static_cast<uint64_t>(0));
		}
		return written;
	}
	uint64_t EventID::Deserialize(IInputStream& stream)
	{
		// Reset the type info as we can't serialize it (not std::type_info type info anyway)
		m_TypeInfo = nullptr;

		uint64_t index = 0;
		uint64_t read = Serialization::ReadObject(stream, index);
		switch (index)
		{
			case 0:
			{
				int64_t value = 0;
				read += Serialization::ReadObject(stream, value);
				m_ID = value;

				break;
			}
			case 1:
			{
				UniversallyUniqueID value;
				read += Serialization::ReadObject(stream, value);
				m_ID = std::move(value);

				break;
			}
			case 2:
			{
				String value;
				read += Serialization::ReadObject(stream, value);
				m_ID = std::move(value);

				break;
			}
			default:
			{
				throw BinarySerializerException(__FUNCTION__ ": Invalid type index");
			}
		};
		return read;
	}

	bool EventID::IsNull() const noexcept
	{
		if (m_ID.valueless_by_exception() || m_ID.index() == std::numeric_limits<size_t>::max())
		{
			return true;
		}
		else if (auto value = std::get_if<int64_t>(&m_ID))
		{
			return *value == 0 || *value == wxEVT_NULL;
		}
		else if (auto value = std::get_if<UniversallyUniqueID>(&m_ID))
		{
			return value->IsNull();
		}
		else if (auto value = std::get_if<String>(&m_ID))
		{
			return value->IsEmpty();
		}
		return false;
	}

	int64_t EventID::AsInt() const noexcept
	{
		if (auto value = std::get_if<int64_t>(&m_ID))
		{
			return *value;
		}
		return 0;
	}
	UniversallyUniqueID EventID::AsUniqueID() const noexcept
	{
		if (auto value = std::get_if<UniversallyUniqueID>(&m_ID))
		{
			return *value;
		}
		return {};
	}
	const String& EventID::AsString() const noexcept
	{
		if (auto value = std::get_if<String>(&m_ID))
		{
			return *value;
		}
		return NullString;
	}

	#ifdef __WXWINDOWS__
	bool EventID::IsWxWidgetsID() const noexcept
	{
		int64_t id = AsInt();
		return id >= wxEVT_FIRST && id <= g_LastWxEventID;
	}
	#endif
}

namespace kxf::EventSystem
{
	EventID NewSimpleEventID() noexcept
	{
		return g_LastWxEventID + (++g_SimpleEventID);
	}
	EventID NewUniqueEventID() noexcept
	{
		return UniversallyUniqueID::CreateSequential();
	}
}
