#include "KxfPCH.h"
#include "StandardLocalization.h"
#include "kxf/Application/ICoreApplication.h"

namespace
{
	constexpr kxf::XChar g_MnemonicSuffix[] = kxS("_MM");

	std::optional<kxf::String> DoGetLocalizedString(kxf::String id, kxf::FlagSet<wxStockLabelQueryFlag> flags)
	{
		using namespace kxf;

		if (auto app = ICoreApplication::GetInstance())
		{
			const ILocalizationPackage& localizationPackage = app->GetLocalizationPackage();
			if (flags.Contains(wxStockLabelQueryFlag::wxSTOCK_WITH_MNEMONIC))
			{
				id += g_MnemonicSuffix;
			}

			auto value = localizationPackage.GetItem(id);
			if (!value)
			{
				id.RemoveRight(std::size(g_MnemonicSuffix) - 1);
				value = localizationPackage.GetItem(id);
			}
			if (value)
			{
				return value.GetString();
			}
		}
		return {};
	}
}

namespace kxf::wxWidgets
{
	String LocalizeLabelString(StdID id, FlagSet<wxStockLabelQueryFlag> flags)
	{
		// Special case: the "Cancel" button shouldn't have a mnemonic under Windows
		// for consistency with the native dialogs (which don't use any mnemonic for it
		// because it is already bound to an 'Esc' key implicitly).
		flags.Remove(wxStockLabelQueryFlag::wxSTOCK_WITH_MNEMONIC, id == StdID::Cancel);

		#define STOCKITEM(stockid, defaultLabel)																\
		case StdID::##stockid:																					\
		{																										\
			result = DoGetLocalizedString(wxS(#stockid), flags).value_or(wxS(defaultLabel));					\
			break;																								\
		}

		String result;
		switch (id)
		{
			STOCKITEM(About, "About");
			STOCKITEM(Add, "Add");
			STOCKITEM(Apply, "Apply");
			STOCKITEM(Backward, "Back");
			STOCKITEM(Ignore, "Ignore");
			STOCKITEM(Bold, "Bold");
			STOCKITEM(Bottom, "Bottom");
			STOCKITEM(Cancel, "Cancel");
			STOCKITEM(CDROM, "CD-ROM");
			STOCKITEM(Clear, "Clear");
			STOCKITEM(Close, "Close");
			STOCKITEM(Convert, "Convert");
			STOCKITEM(Copy, "Copy");
			STOCKITEM(Cut, "Cut");
			STOCKITEM(Delete, "Delete");
			STOCKITEM(Down, "Down");
			STOCKITEM(Edit, "Edit");
			STOCKITEM(Execute, "Execute");
			STOCKITEM(Exit, "Quit");
			STOCKITEM(File, "File");
			STOCKITEM(Find, "Find");
			STOCKITEM(First, "First");
			STOCKITEM(Floppy, "Floppy");
			STOCKITEM(Forward, "Forward");
			STOCKITEM(HardDisk, "Hard disk");
			STOCKITEM(Help, "Help");
			STOCKITEM(Home, "Home");
			STOCKITEM(Indent, "Indent");
			STOCKITEM(Index, "Index");
			STOCKITEM(Info, "Info");
			STOCKITEM(Italic, "Italic");
			STOCKITEM(JumpTo, "Jump to");
			STOCKITEM(JustifyCenter, "Centered");
			STOCKITEM(JustifyFill, "Justified");
			STOCKITEM(JustifyLeft, "Align left");
			STOCKITEM(JustifyRight, "Align right");
			STOCKITEM(Last, "Last");
			STOCKITEM(Network, "Network");
			STOCKITEM(New, "New");
			STOCKITEM(No, "No");
			STOCKITEM(None, "None");
			STOCKITEM(OK, "OK");
			STOCKITEM(Open, "Open...");
			STOCKITEM(Paste, "Paste");
			STOCKITEM(Preferences, "Preferences");
			STOCKITEM(Preview, "Print preview...");
			STOCKITEM(Print, "Print...");
			STOCKITEM(Properties, "Properties");
			STOCKITEM(Redo, "Redo");
			STOCKITEM(Refresh, "Refresh");
			STOCKITEM(Remove, "Remove");
			STOCKITEM(Replace, "Replace");
			STOCKITEM(RevertToSaved, "Revert to saved");
			STOCKITEM(Save, "Save");
			STOCKITEM(SaveAs, "Save as");
			STOCKITEM(SelectAll, "Select all");
			STOCKITEM(SelectColor, "Select color");
			STOCKITEM(SelectFont, "Select font");
			STOCKITEM(SortAscending, "Ascending");
			STOCKITEM(SortDescending, "Descending");
			STOCKITEM(SpellCheck, "Spell check");
			STOCKITEM(Stop, "Stop");
			STOCKITEM(Strikethrough, "Strikethrough");
			STOCKITEM(Top, "Top");
			STOCKITEM(Undelete, "Undelete");
			STOCKITEM(Underline, "Underline");
			STOCKITEM(Undo, "Undo");
			STOCKITEM(Unindent, "Unindent");
			STOCKITEM(Up, "Up");
			STOCKITEM(Yes, "Yes");
			STOCKITEM(Zoom100, "Actual size");
			STOCKITEM(ZoomFit, "Zoom to fit");
			STOCKITEM(ZoomIn, "Zoom in");
			STOCKITEM(ZoomOut, "Zoom out");

			STOCKITEM(Move, "Move");
			STOCKITEM(Rename, "Rename");
			STOCKITEM(Error, "Error");
			STOCKITEM(SelectFile, "Select file");
			STOCKITEM(SelectDirectory, "Select directory");
			STOCKITEM(Directory, "Directory");
			STOCKITEM(Search, "Search");
			STOCKITEM(CopyLink, "Copy link");
		};
		#undef STOCKITEM

		if (result.IsEmpty())
		{
			return Format("#{}", id);
		}
		else
		{
			// Accelerators only make sense for the menu items which should have
			// ellipsis too while wxSTOCK_WITHOUT_ELLIPSIS is mostly useful for
			// buttons which shouldn't have accelerators in their labels.
			if (flags.Contains(wxStockLabelQueryFlag::wxSTOCK_WITHOUT_ELLIPSIS))
			{
				String baseLabel;
				if (result.EndsWith("...", &baseLabel) && !baseLabel.IsEmpty())
				{
					result = std::move(baseLabel);
				}
			}

			#if wxUSE_ACCEL
			if (!result.IsEmpty() && flags.Contains(wxStockLabelQueryFlag::wxSTOCK_WITH_ACCELERATOR))
			{
				wxAcceleratorEntry accelerator = wxGetStockAccelerator(ToInt(id));
				if (accelerator.IsOk())
				{
					result += '\t';
					result += accelerator.ToString();
				}
			}
			#endif
			return result;
		}
	}
	String LocalizeHelpString(StdID id, wxStockHelpStringClient client)
	{
		constexpr wxStockLabelQueryFlag flags = wxStockLabelQueryFlag::wxSTOCK_NOFLAGS;

		#define STOCKITEM(stockid, ctx, defaultLabel)															\
        case StdID::##stockid:																					\
		{																										\
			if (client == ctx)																					\
			{																									\
				result = DoGetLocalizedString(wxS(#stockid) "_LONG", flags).value_or(wxS(defaultLabel));	\
			}																									\
			break;																								\
		}

		String result;
		switch (id)
		{
			//These help string should be not too specific as they could be used in completely different programs!
			STOCKITEM(About, wxSTOCK_MENU, "Show about dialog");
			STOCKITEM(Copy, wxSTOCK_MENU, "Copy selection");
			STOCKITEM(Cut, wxSTOCK_MENU, "Cut selection");
			STOCKITEM(Delete, wxSTOCK_MENU, "Delete selection");
			STOCKITEM(Replace, wxSTOCK_MENU, "Replace selection");
			STOCKITEM(Paste, wxSTOCK_MENU, "Paste selection");
			STOCKITEM(Exit, wxSTOCK_MENU, "Quit this program");
			STOCKITEM(Undo, wxSTOCK_MENU, "Undo last action");
			STOCKITEM(Redo, wxSTOCK_MENU, "Redo last action");
			STOCKITEM(Close, wxSTOCK_MENU, "Close current document");
			STOCKITEM(Save, wxSTOCK_MENU, "Save current document");
			STOCKITEM(SaveAs, wxSTOCK_MENU, "Save current document with a different filename");
		};
		#undef STOCKITEM

		if (result.IsEmpty())
		{
			return Format("#{}", id);
		}
		return result;
	}
}
