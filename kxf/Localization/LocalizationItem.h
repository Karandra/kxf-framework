#pragma once
#include "Common.h"
#include <variant>
#include <map>

namespace kxf
{
	class ILocalizationPackage;

	enum class LocalizationItemKind
	{
		None = 0,

		SingleItem,
		MultipleItems,
		Plurals
	};
	enum class LocalizationItemQuantity
	{
		One,
		Few,
		Many,
		Other
	};

	enum class LocalizationItemFlag: uint32_t
	{
		None = 0,

		Translatable = 1 << 1,
		ConstrainedLength = 1 << 2,
		WithComment = 1 << 3
	};
	kxf_FlagSet_Declare(LocalizationItemFlag);
}

namespace kxf
{
	class KXF_API LocalizationItem final
	{
		public:
			using TSingleItem = String;
			using TMultipleItems = std::vector<TSingleItem>;
			using TPlurals = std::map<LocalizationItemQuantity, TSingleItem>;

		private:
			std::variant<void*, TSingleItem, TMultipleItems, TPlurals> m_Value;
			ILocalizationPackage* m_Package = nullptr;

			FlagSet<LocalizationItemFlag> m_Flags;
			String m_Comment;
			int m_MaxLength = -1;

		public:
			LocalizationItem() = default;
			LocalizationItem(ILocalizationPackage& package, TSingleItem value, FlagSet<LocalizationItemFlag> flags = {}) noexcept
				:m_Package(&package), m_Flags(flags)
			{
				if (!value.IsEmpty())
				{
					m_Value = std::move(value);
				}
			}
			LocalizationItem(ILocalizationPackage& package, TMultipleItems items, FlagSet<LocalizationItemFlag> flags = {}) noexcept
				:m_Package(&package), m_Flags(flags)
			{
				if (!items.empty())
				{
					for (const auto& item: items)
					{
						if (item.IsEmpty())
						{
							return;
						}
					}
					m_Value = std::move(items);
				}
			}
			LocalizationItem(ILocalizationPackage& package, TPlurals items, FlagSet<LocalizationItemFlag> flags = {}) noexcept
				:m_Package(&package), m_Flags(flags)
			{
				if (!items.empty())
				{
					for (const auto& [id, item]: items)
					{
						if (item.IsEmpty())
						{
							return;
						}
					}
					m_Value = std::move(items);
				}
			}
			LocalizationItem(const LocalizationItem&) = default;
			LocalizationItem(LocalizationItem&&) noexcept = default;

		public:
			bool IsNull() const noexcept;
			size_t GetItemCount() const noexcept;
			ILocalizationPackage& GetPackage() const noexcept
			{
				return *m_Package;
			}

			LocalizationItemKind GetKind() const noexcept
			{
				if (!m_Value.valueless_by_exception() && m_Value.index() != 0)
				{
					static_assert(static_cast<int>(LocalizationItemKind::None) == 0, "Value of 'LocalizationItemKind::None' must be zero");
					return static_cast<LocalizationItemKind>(m_Value.index());
				}
				return LocalizationItemKind::None;
			}
			FlagSet<LocalizationItemFlag> GetFlags() const noexcept
			{
				return m_Flags;
			}

			const String& GetComment() const&
			{
				return m_Comment;
			}
			String GetComment() &&
			{
				return std::move(m_Comment);
			}
			void SetComment(String comment)
			{
				m_Flags.Mod(LocalizationItemFlag::WithComment, !comment.IsEmpty());
				m_Comment = std::move(comment);
			}

			int GetMaxLength() const noexcept
			{
				return m_MaxLength;
			}
			void SetMaxLength(int maxLength) noexcept
			{
				m_Flags.Mod(LocalizationItemFlag::ConstrainedLength, maxLength >= 0);
				m_MaxLength = std::clamp(maxLength, -1, std::numeric_limits<int>::max());
			}

		public:
			const String& GetString(size_t index = 0) const& noexcept;
			String GetString(size_t index = 0) && noexcept;

			const String& GetPluralString(LocalizationItemQuantity quantity) const noexcept;
			const String& GetPluralString(int quantity) const noexcept;

			template<class TFunc>
			size_t EnumStrings(TFunc&& func) const noexcept(std::is_nothrow_invocable_v<TFunc, const TSingleItem&>)
			{
				if (auto value = std::get_if<TSingleItem>(&m_Value))
				{
					if (!value->IsEmpty())
					{
						std::invoke(func, *value);
					}
				}
				else if (auto items = std::get_if<TMultipleItems>(&m_Value))
				{
					size_t count = 0;
					for (const TSingleItem& item: items)
					{
						count++;
						if (!std::invoke(func, item))
						{
							break;
						}
					}
					return count;
				}
				return 0;
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

			operator const String&() const noexcept
			{
				return GetString();
			}
			const String& operator[](size_t index) const noexcept
			{
				return GetString(index);
			}

			bool operator==(const LocalizationItem& other) const noexcept
			{
				if (this != &other)
				{
					return m_Package == other.m_Package &&
						m_Flags == other.m_Flags &&
						m_MaxLength == other.m_MaxLength &&
						m_Comment == other.m_Comment &&
						m_Value == other.m_Value;
				}
				return true;
			}

			LocalizationItem& operator=(const LocalizationItem&) = default;
			LocalizationItem& operator=(LocalizationItem&&) noexcept = default;
	};
}

namespace kxf
{
	KXF_API extern const LocalizationItem NullLocalizationItem;
}
