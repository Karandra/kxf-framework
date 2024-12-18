#pragma once
#include "Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/AlignedStorage.h"
#include "kxf/DateTime/DateTime.h"
class wxCmdLineParser;

namespace kxf
{
	enum class CommandLineParserResult
	{
		Success = 0,
		HelpRequested,
		SyntaxError
	};
	enum class CommandLineParserSwitch
	{
		None = -1,

		Enabled,
		Disabled
	};
	enum class CommandLineParserValueType
	{
		None = -1,

		String,
		DateTime,
		FloatingPoint,
		Integer
	};

	enum class CommandLineParserFlag: uint32_t
	{
		None = 0,

		ParameterOpional = FlagSetValue<CommandLineParserFlag>(1),
		ParameterMultiple = FlagSetValue<CommandLineParserFlag>(2),

		OptionHelp = FlagSetValue<CommandLineParserFlag>(3),
		OptionRequired = FlagSetValue<CommandLineParserFlag>(4),
		OptionWithSeparator = FlagSetValue<CommandLineParserFlag>(5),

		SwitchHidden = FlagSetValue<CommandLineParserFlag>(6),
		SwitchNegatable = FlagSetValue<CommandLineParserFlag>(7)
	};
	kxf_FlagSet_Declare(CommandLineParserFlag);
}

namespace kxf
{
	class KXF_API CommandLineArg final
	{
		friend class CommandLineParser;

		private:
			String m_LongName;
			String m_ShortName;
			std::variant<String, DateTime, int64_t, double> m_Value;
			bool m_IsNegated = false;

		public:
			CommandLineArg() = default;
			~CommandLineArg() = default;

		public:
			const String& GetLongName() const noexcept
			{
				return m_LongName;
			}
			const String& GetShortName() const noexcept
			{
				return m_ShortName;
			}
			bool IsNegated() const noexcept
			{
				return m_IsNegated;
			}

			std::optional<String> GetStringValue() const
			{
				if (auto value = std::get_if<String>(&m_Value))
				{
					return *value;
				}
				return {};
			}
			std::optional<DateTime> GetDateTimeValue() const
			{
				if (auto value = std::get_if<DateTime>(&m_Value))
				{
					return *value;
				}
				return {};
			}
			std::optional<double> GetFloatingPointValue() const
			{
				if (auto value = std::get_if<double>(&m_Value))
				{
					return *value;
				}
				return {};
			}
			std::optional<int64_t> GetIntegerValue() const
			{
				if (auto value = std::get_if<int64_t>(&m_Value))
				{
					return *value;
				}
				return {};
			}
	};
}

namespace kxf
{
	class KXF_API CommandLineParser final
	{
		private:
			void MoveFrom(CommandLineParser& other) noexcept;

		private:
			AlignedStorage<wxCmdLineParser, sizeof(void*), alignof(void*)> m_Parser;

		public:
			CommandLineParser();
			CommandLineParser(int argc, char** argv);
			CommandLineParser(int argc, wchar_t** argv);
			CommandLineParser(const String& commandLine);
			CommandLineParser(const CommandLineParser&) = delete;
			CommandLineParser(CommandLineParser&& other) noexcept;
			~CommandLineParser() noexcept;

		public:
			bool IsNull() const noexcept;

			bool IsLongNamesEnabled() const;
			void SetLongNamesEnabled(bool enable = true);
			void SetSwitchCharacters(const String& switchCharacters);

			bool FindCommandLine();
			void SetCommandLine(int argc, char** argv);
			void SetCommandLine(int argc, wchar_t** argv);
			void SetCommandLine(const String& commandLine);
			CommandLineParserResult Parse(bool showUsage = false);

			String GetUsage() const;
			void ShowUsage();
			void AddUsageText(const String& usage);
			void AddUsageLogo(const String& logo);

			void AddParameter(const String& description = {}, CommandLineParserValueType type = CommandLineParserValueType::String, FlagSet<CommandLineParserFlag> flags = {});
			void AddOption(const String& name, const String& longName = {}, const String& description = {}, CommandLineParserValueType type = CommandLineParserValueType::String, FlagSet<CommandLineParserFlag> flags = {});
			void AddSwitch(const String& name, const String& longName = {}, const String& description = {}, FlagSet<CommandLineParserFlag> flags = {});

			size_t GetParameterCount() const;
			String GetParameterAt(size_t index) const;
			Enumerator<String> EnumParameters() const;
			Enumerator<CommandLineArg> EnumArguments() const;

			std::optional<String> GetStringOption(const String& name) const;
			std::optional<DateTime> GetDateTimeOption(const String& name) const;
			std::optional<double> GetFloatingPointOption(const String& name) const;
			std::optional<int64_t> GetIntegerOption(const String& name) const;

			CommandLineParserSwitch GetSwitch(const String& name) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			CommandLineParser& operator=(const CommandLineParser&) = delete;
			CommandLineParser& operator=(CommandLineParser&& other) noexcept;
	};
}
