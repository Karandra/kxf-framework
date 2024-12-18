#include "kxf-pch.h"
#include "CommandLineParser.h"
#include "kxf/Utility/Enumerator.h"
#include "kxf/wxWidgets/Setup.h"
#include "wx/cmdline.h"

namespace
{
	constexpr wxCmdLineParamType MapParameterType(kxf::CommandLineParserValueType type) noexcept
	{
		using namespace kxf;

		switch (type)
		{
			case CommandLineParserValueType::String:
			{
				return wxCMD_LINE_VAL_STRING;
			}
			case CommandLineParserValueType::DateTime:
			{
				return wxCMD_LINE_VAL_STRING;
			}
			case CommandLineParserValueType::FloatingPoint:
			{
				return wxCMD_LINE_VAL_DOUBLE;
			}
			case CommandLineParserValueType::Integer:
			{
				return wxCMD_LINE_VAL_NUMBER;
			}
		};
		return wxCMD_LINE_VAL_NONE;
	}
	constexpr kxf::FlagSet<wxCmdLineEntryFlags> MapFlags(kxf::FlagSet<kxf::CommandLineParserFlag> flags) noexcept
	{
		using namespace kxf;

		FlagSet<wxCmdLineEntryFlags> flagsWx;
		flagsWx.Add(wxCmdLineEntryFlags::wxCMD_LINE_PARAM_OPTIONAL, flags & CommandLineParserFlag::ParameterOpional);
		flagsWx.Add(wxCmdLineEntryFlags::wxCMD_LINE_PARAM_MULTIPLE, flags & CommandLineParserFlag::ParameterMultiple);

		flagsWx.Add(wxCmdLineEntryFlags::wxCMD_LINE_OPTION_HELP, flags & CommandLineParserFlag::OptionHelp);
		flagsWx.Add(wxCmdLineEntryFlags::wxCMD_LINE_OPTION_MANDATORY, flags & CommandLineParserFlag::OptionRequired);
		flagsWx.Add(wxCmdLineEntryFlags::wxCMD_LINE_NEEDS_SEPARATOR, flags & CommandLineParserFlag::OptionWithSeparator);

		flagsWx.Add(wxCmdLineEntryFlags::wxCMD_LINE_HIDDEN, flags & CommandLineParserFlag::SwitchHidden);
		flagsWx.Add(wxCmdLineEntryFlags::wxCMD_LINE_SWITCH_NEGATABLE, flags & CommandLineParserFlag::SwitchNegatable);

		return flagsWx;
	}

	struct HackWxCmdLineParser final
	{
		struct wxCmdLineParserData* m_Impl = nullptr;
	};
}

namespace kxf
{
	void CommandLineParser::MoveFrom(CommandLineParser& other) noexcept
	{
		auto thisPtr = reinterpret_cast<HackWxCmdLineParser*>(m_Parser.get());
		auto otherPtr = reinterpret_cast<HackWxCmdLineParser*>(other.m_Parser.get());

		// Construct an uninitialized object or destroy and reconstruct an already used one
		if (!m_Parser.IsConstructed())
		{
			m_Parser.Construct();
		}
		else if (thisPtr->m_Impl)
		{
			m_Parser.Destroy();
			m_Parser.Construct();
		}

		if (other.m_Parser.IsConstructed())
		{
			thisPtr->m_Impl = otherPtr->m_Impl;
			otherPtr->m_Impl = nullptr;
		}
	}

	CommandLineParser::CommandLineParser()
	{
		m_Parser.Construct();
	}
	CommandLineParser::CommandLineParser(CommandLineParser&& other) noexcept
	{
		MoveFrom(other);
	}
	CommandLineParser::CommandLineParser(int argc, char** argv)
	{
		m_Parser.Construct(argc, argv);
	}
	CommandLineParser::CommandLineParser(int argc, wchar_t** argv)
	{
		m_Parser.Construct(argc, argv);
	}
	CommandLineParser::CommandLineParser(const String& commandLine)
	{
		m_Parser.Construct(commandLine);
	}
	CommandLineParser::~CommandLineParser() noexcept
	{
		m_Parser.Destroy();
	}

	bool CommandLineParser::IsNull() const noexcept
	{
		return !m_Parser.IsConstructed();
	}

	bool CommandLineParser::IsLongNamesEnabled() const
	{
		return m_Parser->AreLongOptionsEnabled();
	}
	void CommandLineParser::SetLongNamesEnabled(bool enable)
	{
		m_Parser->EnableLongOptions(enable);
	}
	void CommandLineParser::SetSwitchCharacters(const String& switchCharacters)
	{
		m_Parser->SetSwitchChars(switchCharacters);
	}

	bool CommandLineParser::FindCommandLine()
	{
		if (auto commandLine = ::GetCommandLineW())
		{
			SetCommandLine(commandLine);
			return true;
		}
		return false;
	}
	void CommandLineParser::SetCommandLine(int argc, char** argv)
	{
		m_Parser->SetCmdLine(argc, argv);
	}
	void CommandLineParser::SetCommandLine(int argc, wchar_t** argv)
	{
		m_Parser->SetCmdLine(argc, argv);
	}
	void CommandLineParser::SetCommandLine(const String& commandLine)
	{
		m_Parser->SetCmdLine(commandLine);
	}
	CommandLineParserResult CommandLineParser::Parse(bool showUsage)
	{
		switch (m_Parser->Parse(showUsage))
		{
			case 0:
			{
				return CommandLineParserResult::Success;
			}
			case -1:
			{
				return CommandLineParserResult::HelpRequested;
			}
		};
		return CommandLineParserResult::SyntaxError;
	}

	String CommandLineParser::GetUsage() const
	{
		return m_Parser->GetUsageString();
	}
	void CommandLineParser::ShowUsage()
	{
		m_Parser->Usage();
	}
	void CommandLineParser::AddUsageText(const String& usage)
	{
		m_Parser->AddUsageText(usage);
	}
	void CommandLineParser::AddUsageLogo(const String& logo)
	{
		m_Parser->SetLogo(logo);
	}

	void CommandLineParser::AddParameter(const String& description, CommandLineParserValueType type, FlagSet<CommandLineParserFlag> flags)
	{
		m_Parser->AddParam(description, MapParameterType(type), *MapFlags(flags));
	}
	void CommandLineParser::AddOption(const String& name, const String& longName, const String& description, CommandLineParserValueType type, FlagSet<CommandLineParserFlag> flags)
	{
		m_Parser->AddOption(name, longName, description, MapParameterType(type), *MapFlags(flags));
	}
	void CommandLineParser::AddSwitch(const String& name, const String& longName, const String& description, FlagSet<CommandLineParserFlag> flags)
	{
		m_Parser->AddSwitch(name, longName, description, *MapFlags(flags));
	}

	size_t CommandLineParser::GetParameterCount() const
	{
		return m_Parser->GetParamCount();
	}
	String CommandLineParser::GetParameterAt(size_t index) const
	{
		return m_Parser->GetParam(index);
	}
	Enumerator<String> CommandLineParser::EnumParameters() const
	{
		return [this, index = 0_uz]() -> std::optional<String>
		{
			if (index < m_Parser->GetParamCount())
			{
				return m_Parser->GetParam(index);
			}
			return {};
		};
	}
	Enumerator<CommandLineArg> CommandLineParser::EnumArguments() const
	{
		return Utility::EnumerateIterableContainer<CommandLineArg>(m_Parser->GetArguments(), [](const wxCmdLineArg& argWx)
		{
			CommandLineArg arg;
			arg.m_LongName = argWx.GetLongName();
			arg.m_ShortName = argWx.GetShortName();
			arg.m_IsNegated = argWx.IsNegated();

			switch (argWx.GetType())
			{
				case wxCMD_LINE_VAL_STRING:
				{
					arg.m_Value = String(argWx.GetStrVal());
					break;
				}
				case wxCMD_LINE_VAL_DATE:
				{
					arg.m_Value = DateTime(argWx.GetDateVal());
					break;
				}
				case wxCMD_LINE_VAL_DOUBLE:
				{
					arg.m_Value = static_cast<double>(argWx.GetDoubleVal());
					break;
				}
				case wxCMD_LINE_VAL_NUMBER:
				{
					arg.m_Value = static_cast<int64_t>(argWx.GetLongVal());
					break;
				}
			};
			return arg;
		});
	}

	std::optional<String> CommandLineParser::GetStringOption(const String& name) const
	{
		wxString value;
		if (m_Parser->Found(name, &value))
		{
			return value;
		}
		return {};
	}
	std::optional<DateTime> CommandLineParser::GetDateTimeOption(const String& name) const
	{
		wxDateTime value;
		if (m_Parser->Found(name, &value))
		{
			return value;
		}
		return {};
	}
	std::optional<double> CommandLineParser::GetFloatingPointOption(const String& name) const
	{
		double value;
		if (m_Parser->Found(name, &value))
		{
			return value;
		}
		return {};
	}
	std::optional<int64_t> CommandLineParser::GetIntegerOption(const String& name) const
	{
		long value;
		if (m_Parser->Found(name, &value))
		{
			return value;
		}
		return {};
	}

	CommandLineParserSwitch CommandLineParser::GetSwitch(const String& name) const
	{
		switch (m_Parser->FoundSwitch(name))
		{
			case wxCmdLineSwitchState::wxCMD_SWITCH_OFF:
			{
				return CommandLineParserSwitch::Disabled;
			}
			case wxCmdLineSwitchState::wxCMD_SWITCH_ON:
			{
				return CommandLineParserSwitch::Enabled;
			}
		};
		return CommandLineParserSwitch::None;
	}

	CommandLineParser& CommandLineParser::operator=(CommandLineParser&& other) noexcept
	{
		MoveFrom(other);
		return *this;
	}
}
