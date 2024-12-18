#include "kxf-pch.h"
#include "IVariableCollection.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/System/ShellOperations.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/Utility/ScopeGuard.h"

namespace
{
	kxf::String DoExpandStdVariables(const kxf::String& source, const kxf::IVariableCollection* collection = nullptr)
	{
		using namespace kxf;

		return ExpandVariables(source, [&](const String& ns, const String& id) -> String
		{
			if (ns == kxS("LCIT"))
			{
				if (auto app = ICoreApplication::GetInstance())
				{
					return app->GetLocalizationPackage().GetItem(id).GetString();
				}
			}
			else if (ns == kxS("ENV"))
			{
				return System::GetEnvironmentVariable(id);
			}
			else if (ns == kxS("SHDir"))
			{
				KnownDirectoryID desiredDirectoryID = KnownDirectoryID::None;
				Shell::EnumKnownDirectories([&](KnownDirectoryID directoryID, String directoryName)
				{
					if (directoryName == id)
					{
						directoryID = directoryID;
						return false;
					}
					return true;
				});

				return Shell::GetKnownDirectory(desiredDirectoryID);
			}
			else if (collection)
			{
				return collection->GetItem(ns, id).GetAs<String>();
			}
			return {};
		});
	}
}

namespace kxf
{
	String ExpandVariables(const String& source, std::function<String(const String& ns, const String& id)> onVariable)
	{
		if (!source.IsEmpty())
		{
			String result = source;
			size_t entryStartPos = String::npos;
			size_t varNameStartPos = String::npos;

			for (size_t i = 0; i < result.length(); i++)
			{
				// Find the variable anchor
				if (result[i] == '$')
				{
					entryStartPos = i;
				}

				// We're at the beginning of the variable name
				if (entryStartPos != String::npos && result[i] == '(')
				{
					varNameStartPos = i + 1;
				}

				// We've found the end of the variable, extract namespace and call the provided callback
				if (entryStartPos != String::npos && varNameStartPos != String::npos && result[i] == ')')
				{
					Utility::ScopeGuard atExit = [&]()
					{
						// Reset parser state
						entryStartPos = String::npos;
						varNameStartPos = String::npos;
					};

					const size_t varEntryLength = i - entryStartPos + 1;
					const size_t varNameLength = i - varNameStartPos;
					if (varNameLength != 0 && varNameStartPos >= 2)
					{
						const String varNamespace = result.SubRange(entryStartPos + 1, varNameStartPos - 2);
						const String varName = result.SubMid(varNameStartPos, varNameLength);

						String value = std::invoke(onVariable, varNamespace, varName);
						if (!value.IsEmpty())
						{
							result.ReplaceRange(entryStartPos, varEntryLength, value);

							// Set loop counter to end of replacement
							i = entryStartPos + value.length() - 1;
						}
					}
				}
			}
			return result;
		}
		return source;
	}
	String ExpandVariables(const String& source, const IVariableCollection& collection)
	{
		return ExpandVariables(source, [&](const String& ns, const String& id) -> String
		{
			return collection.GetItem(ns, id).GetAs<String>();
		});
	}

	String ExpandStdVariables(const String& source)
	{
		return DoExpandStdVariables(source);
	}
	String ExpandStdVariables(const String& source, const IVariableCollection& collection)
	{
		return DoExpandStdVariables(source, &collection);
	}
}
