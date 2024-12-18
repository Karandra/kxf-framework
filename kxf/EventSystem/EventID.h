#pragma once
#include "Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/Serialization/BinarySerializer.h"
#include "kxf/Utility/Memory.h"
#include <type_traits>
#include <variant>

namespace kxf
{
	class IEvent;
}

namespace kxf
{
	class KXF_API EventID final
	{
		friend struct std::hash<EventID>;
		friend struct BinarySerializer<EventID>;

		private:
			std::variant<int64_t, UniversallyUniqueID, String> m_ID;
			const std::type_info* m_TypeInfo = nullptr;

		private:
			size_t GetHash() const noexcept;
			uint64_t Serialize(IOutputStream& stream) const;
			uint64_t Deserialize(IInputStream& stream);

		public:
			EventID() noexcept = default;
			EventID(bool) = delete;

			// Null pointer
			EventID(std::nullptr_t) noexcept
				:m_ID(static_cast<int64_t>(0))
			{
			}

			// Member function pointer
			template<class TFunc>
			requires(std::is_member_function_pointer_v<TFunc>)
			EventID(TFunc func) noexcept
				:m_ID(UniversallyUniqueID::CreateFromInt128(Utility::StoreMemberFunction(func)))
			{
			}
			
			// Simple integer or enum
			template<class T>
			requires(std::is_integral_v<T> || std::is_enum_v<T>)
			EventID(T id) noexcept
				:m_ID(static_cast<int64_t>(id))
			{
			}
			
			// UUID
			EventID(UniversallyUniqueID id) noexcept
				:m_ID(std::move(id))
			{
			}
			
			// String
			EventID(String id) noexcept
				:m_ID(std::move(id))
			{
			}
			EventID(const char* id) noexcept
				:m_ID(String(id))
			{
			}
			EventID(const wchar_t* id) noexcept
				:m_ID(String(id))
			{
			}
			
			// wxWidgets event tag (integer)
			#ifdef __WXWINDOWS__
			template<class T>
			EventID(const wxEventTypeTag<T>& eventTag) noexcept
				:m_ID(static_cast<wxEventType>(eventTag))
			{
			}
			#endif

			EventID(EventID&& other) noexcept
			{
				*this = std::move(other);
			}
			EventID(const EventID&) = default;

		public:
			bool IsNull() const noexcept;

			int64_t AsInt() const noexcept;
			UniversallyUniqueID AsUniqueID() const noexcept;
			const String& AsString() const noexcept;

			bool HasEventClassInfo() const noexcept
			{
				return m_TypeInfo != nullptr;
			}

			template<class TEvent>
			requires(std::is_base_of_v<IEvent, TEvent>)
			bool IsOfEventClass() const noexcept
			{
				if (std::is_same_v<TEvent, IEvent>)
				{
					return m_TypeInfo == nullptr;
				}
				else
				{
					return m_TypeInfo && *m_TypeInfo == typeid(TEvent);
				}
			}

			template<class TEvent>
			requires(std::is_base_of_v<IEvent, TEvent>)
			void AssignEventClass() noexcept
			{
				if (!std::is_same_v<TEvent, IEvent>)
				{
					m_TypeInfo = &typeid(TEvent);
				}
				else
				{
					m_TypeInfo = nullptr;
				}
			}

			#ifdef __WXWINDOWS__
			bool IsWxWidgetsID() const noexcept;
			#endif

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			auto operator<=>(const EventID& other) const noexcept
			{
				return m_ID <=> other.m_ID;
			}
			bool operator==(const EventID& other) const noexcept
			{
				return m_ID == other.m_ID;
			}
			bool operator!=(const EventID& other) const noexcept
			{
				return m_ID != other.m_ID;
			}

			EventID& operator=(EventID&& other) noexcept
			{
				m_ID = std::move(other.m_ID);
				m_TypeInfo = std::exchange(other.m_TypeInfo, nullptr);

				return *this;
			}
			EventID& operator=(const EventID&) = default;
	};
}

namespace kxf
{
	template<class TEvent_>
	class EventTag final
	{
		public:
			using TEvent = TEvent_;

		private:
			EventID m_ID;

		private:
			void Init() noexcept
			{
				if constexpr(std::is_base_of_v<IEvent, TEvent>)
				{
					m_ID.AssignEventClass<TEvent>();
				}
				else
				{
					m_ID.AssignEventClass<IEvent>();
				}
			}

		public:
			EventTag() noexcept = default;
			EventTag(EventID id) noexcept
				:m_ID(std::move(id))
			{
				Init();
			}

			template<class T> requires(std::is_constructible_v<EventID, T>)
			EventTag(T&& arg) noexcept(std::is_nothrow_constructible_v<EventID, T>)
				:m_ID(std::forward<T>(arg))
			{
				Init();
			}

			EventTag(EventTag&& other) noexcept
				:m_ID(std::move(other))
			{
			}
			EventTag(const EventTag&) = default;

		public:
			bool IsNull() const noexcept
			{
				return m_ID.IsNull();
			}
			#ifdef __WXWINDOWS__
			wxEventTypeTag<TEvent> ToWxTag() const noexcept
			{
				return m_ID.AsInt();
			}
			#endif

			const EventID& operator*() const noexcept
			{
				return m_ID;
			}
			EventID& operator*() noexcept
			{
				return m_ID;
			}

			const EventID* operator&() const noexcept
			{
				return &m_ID;
			}
			EventID* operator&() noexcept
			{
				return &m_ID;
			}

			const EventID* operator->() const noexcept
			{
				return &m_ID;
			}
			EventID* operator->() noexcept
			{
				return &m_ID;
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			operator const EventID&() const noexcept
			{
				return m_ID;
			}
			operator EventID&() noexcept
			{
				return m_ID;
			}

			auto operator<=>(const EventTag&) const noexcept = default;
			bool operator==(const EventTag&) const noexcept = default;

			auto operator<=>(const EventID& other) const noexcept
			{
				return m_ID <=> other;
			}
			bool operator==(const EventID& other) const noexcept
			{
				return m_ID == other;
			}

			EventTag& operator=(EventTag&& other) noexcept
			{
				m_ID = std::move(other.m_ID);
				return *this;
			}
			EventTag& operator=(const EventTag&) = default;
	};
}

namespace kxf::EventSystem
{
	KXF_API EventID NewSimpleEventID() noexcept;
	KXF_API EventID NewUniqueEventID() noexcept;
}

namespace kxf
{
	template<>
	struct BinarySerializer<EventID> final
	{
		uint64_t Serialize(IOutputStream& stream, const EventID& value) const
		{
			return value.Serialize(stream);
		}
		uint64_t Deserialize(IInputStream& stream, EventID& value) const
		{
			return value.Deserialize(stream);
		}
	};
}

namespace std
{
	template<>
	struct hash<kxf::EventID> final
	{
		size_t operator()(const kxf::EventID& id) const noexcept
		{
			return id.GetHash();
		}
	};
}
