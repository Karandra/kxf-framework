#include "kxf-pch.h"
#include "IGraphicsContext.h"
#include "../GDIRenderer/GDIBitmap.h"
#include "../GDIRenderer/GDIMemoryContext.h"
#include <wx/scrolwin.h>

namespace
{
	constexpr auto g_EllipseReplacement = "...";
}

namespace
{
	using namespace kxf;

	class EllipsizeCalculator final
	{
		private:
			// Indexes inside the string have a valid range of [0; len - 1] if not otherwise constrained
			// lengths/counts of characters (e.g. nCharsToRemove) have a valid range of [0; len] if not
			// otherwise constrained.

			// Since this point we know we have for sure a non-empty string from which we need
			// to remove _at least_ one character (thus nCharsToRemove below is constrained to be >= 1).

			// Index of first character to erase, valid range is [0; len - 1]:
			size_t m_InitialCharToRemove = 0;
			// How many chars do we need to erase? valid range is [0; len - m_InitialCharToRemove]
			size_t m_CharsToRemove = 0;

			String m_ResultText;
			bool m_ResultTextNeedsUpdate = true;

			const IGraphicsContext& m_GC;
			String m_SourceText;
			float m_MaxWidth;
			float m_ReplacementWidth;
			std::vector<float> m_CharOffsets;

			bool m_IsValid = false;

		public:
			EllipsizeCalculator(const String& text, const IGraphicsContext& gc, float maxWidth, float replacementWidth, FlagSet<EllipsizeFlag> flags, const IGraphicsFont& font)
				:m_SourceText(text), m_GC(gc), m_MaxWidth(maxWidth), m_ReplacementWidth(replacementWidth)
			{
				size_t expectedOffsetsCount = text.length();

				// Where ampersands are used as mnemonic indicator they should not affect the overall width of the string
				// and must be removed from the measurement. Nonetheless, we need to keep them in the string and have a
				// corresponding entry in 'm_CharOffsets'.
				if (flags.Contains(EllipsizeFlag::ProcessMnemonics))
				{
					// Create a copy of the string with the ampersands removed to get the correct widths.
					const String sourceCopy = wxControl::RemoveMnemonics(text);
					m_CharOffsets = gc.GetPartialTextExtents(sourceCopy, font);

					// Iterate through the original string inserting a cumulative width value for each ampersand
					// that is the same as the following character's cumulative width value. Except this is only done
					// for the first ampersand in a pair (see 'wxControl::RemoveMnemonics').
					size_t n = 0;
					bool lastWasMnemonic = false;
					for (auto it = text.begin(); it != text.end(); ++it, ++n)
					{
						if (*it == '&' && !lastWasMnemonic)
						{
							if (it + 1 != text.end())
							{
								float width = m_CharOffsets[n];
								m_CharOffsets.insert(m_CharOffsets.begin() + n, width);
								lastWasMnemonic = true;
							}
							else
							{
								// Last character is an ampersand. This ampersand is removed by 'RemoveMnemonics' and won't be displayed
								// when this string is drawn neither, so we intentionally don't use it for our calculations neither,
								// just account for this in the check below.
								expectedOffsetsCount--;
							}
						}
						else
						{
							// Not an ampersand used to introduce a mnemonic.
							lastWasMnemonic = false;
						}
					}
				}
				else
				{
					m_CharOffsets = gc.GetPartialTextExtents(text, font);
				}

				// Either way, we should end up with the same number of offsets as characters in the original string.
				m_IsValid = !m_CharOffsets.empty() && m_CharOffsets.size() == expectedOffsetsCount;
			}

		public:
			void Initialize(size_t initialCharToRemove, size_t nCharsToRemove)
			{
				m_InitialCharToRemove = initialCharToRemove;
				m_CharsToRemove = nCharsToRemove;
			}
			String& GetEllipsizedText()
			{
				if (m_ResultTextNeedsUpdate)
				{
					wxASSERT(m_InitialCharToRemove <= m_SourceText.length() - 1); // See valid range for 'm_InitialCharToRemove' above
					wxASSERT(m_CharsToRemove >= 1 && m_CharsToRemove <= m_SourceText.length() - m_InitialCharToRemove);  // See valid range for 'm_CharsToRemove' above

					// Erase 'm_CharsToRemove' characters after 'm_InitialCharToRemove' (included)
					// e.g. if we have the string "foobar" (length = 6)
					//                               ^
					//                               \--- 'm_InitialCharToRemove' = 2
					//      and 'm_CharsToRemove' = 2, then we get "foar"
					m_ResultText = m_SourceText;
					m_ResultText.ReplaceRange(m_InitialCharToRemove, m_CharsToRemove, StringViewOf(g_EllipseReplacement));
				}
				return m_ResultText;
			}

			bool IsShortEnough()
			{
				if (m_CharsToRemove == m_SourceText.length())
				{
					// that's the best we could do
					return true;
				}

				// Width calculation using partial extents is just an inaccurate
				// estimate: partial extents have sub-pixel precision and are rounded
				// by 'GetPartialTextExtents'. Replacing part of the string with "..."
				// may change them too thanks to changes in ligatures, kerning etc.

				// The correct algorithm would be to call 'GetTextExtent' in every step
				// of ellipsize algorithm, but that would be too expensive, especially when
				// the difference is just a few pixels. So we use partial extents to
				// estimate string width and only verify it with 'GetTextExtent' when
				// it looks good.

				// The length of "..."
				float estimatedWidth = m_ReplacementWidth;

				// Length of text before the removed part
				if (m_InitialCharToRemove > 0)
				{
					estimatedWidth += m_CharOffsets[m_InitialCharToRemove - 1];
				}

				// Length of text after the removed part
				if (GetLastRemoved() < m_SourceText.length())
				{
					estimatedWidth += m_CharOffsets.back() - m_CharOffsets[GetLastRemoved()];
				}

				if (estimatedWidth > m_MaxWidth)
				{
					return false;
				}
				return m_GC.GetTextExtent(GetEllipsizedText()).GetWidth() <= m_MaxWidth;
			}
			bool IsNotRequired() const
			{
				// NOTE: m_CharOffsets[n] is the width in pixels of the first 'n' characters (with the last one *included*)
				// thus m_CharOffsets[length - 1] is the total width of the string.
				return m_CharOffsets.back() <= m_MaxWidth;
			}

			void RemoveFromEnd()
			{
				m_CharsToRemove++;
			}
			void RemoveFromStart()
			{
				m_InitialCharToRemove--;
				m_CharsToRemove++;
			}

			size_t GetCharsToRemove() const
			{
				return m_CharsToRemove;
			}
			size_t GetFirstRemoved() const
			{
				return m_InitialCharToRemove;
			}
			size_t GetLastRemoved() const
			{
				return m_InitialCharToRemove + m_CharsToRemove - 1;
			}

		public:
			explicit operator bool() const
			{
				return m_IsValid;
			}
			bool operator!() const
			{
				return !m_IsValid;
			}
	};

	String DoEllipsizeSingleLine(const String& currentLine,
								 const IGraphicsContext& gc,
								 float maxWidth,
								 EllipsizeMode mode,
								 FlagSet<EllipsizeFlag> flags,
								 const IGraphicsFont& font,
								 float replacementWidth)
	{
		if (maxWidth <= 0)
		{
			return {};
		}

		size_t length = currentLine.length();
		if (length <= 1)
		{
			return currentLine;
		}

		EllipsizeCalculator calc(currentLine, gc, maxWidth, replacementWidth, flags, font);
		if (!calc)
		{
			return currentLine;
		}
		if (calc.IsNotRequired())
		{
			return currentLine;
		}

		// Let's compute the range of characters to remove depending on the ellipsize mode
		switch (mode)
		{
			case EllipsizeMode::Start:
			{
				calc.Initialize(0, 1);
				while (!calc.IsShortEnough())
				{
					calc.RemoveFromEnd();
				}

				// Always show at least one character of the string:
				if (calc.GetCharsToRemove() == length)
				{
					return String::Concat(g_EllipseReplacement, currentLine[length - 1]);
				}
				break;
			}
			case EllipsizeMode::Middle:
			{
				// The following piece of code works also when length == 1

				// Start the removal process from the middle of the string i.e. separate the string in three parts:
				// - the first one to preserve, valid range [0; initialCharToRemove - 1] or the empty range if initialCharToRemove == 0
				// - the second one to remove, valid range [initialCharToRemove; endCharToRemove]
				// - the third one to preserve, valid range [endCharToRemove + 1; length - 1] or the empty range if endCharToRemove == length - 1
				// An empty range != range [0; 0] since the range [0; 0] contains 1 character (the zero-th one).

				bool removeFromStart = true;

				calc.Initialize(length / 2, 0);
				while (!calc.IsShortEnough())
				{
					const bool canRemoveFromStart = calc.GetFirstRemoved() > 0;
					const bool canRemoveFromEnd = calc.GetLastRemoved() < length - 1;

					if (!canRemoveFromStart && !canRemoveFromEnd)
					{
						// We need to remove all the characters of the string.
						break;
					}

					// Remove from the beginning in even steps and from the end in odd steps, unless we exhausted one side already.
					removeFromStart = !removeFromStart;
					if (removeFromStart && !canRemoveFromStart)
					{
						removeFromStart = false;
					}
					else if (!removeFromStart && !canRemoveFromEnd)
					{
						removeFromStart = true;
					}

					if (removeFromStart)
					{
						calc.RemoveFromStart();
					}
					else
					{
						calc.RemoveFromEnd();
					}
				}

				// Always show at least one character of the string. Additionally, if there's only one character left,
				// prefer "a..." to "...a".
				if (calc.GetCharsToRemove() == length || calc.GetCharsToRemove() == length - 1)
				{
					return String::Concat(currentLine[0], g_EllipseReplacement);
				}

				break;
			}
			case EllipsizeMode::End:
			{
				calc.Initialize(length - 1, 1);
				while (!calc.IsShortEnough())
				{
					calc.RemoveFromStart();
				}

				// Always show at least one character of the string
				if (calc.GetCharsToRemove() == length)
				{
					return String::Concat(currentLine[0], g_EllipseReplacement);
				}
				break;
			}
			default:
			{
				return currentLine;
			}
		};
		return std::move(calc.GetEllipsizedText());
	}
}

namespace kxf
{
	GDIBitmap IGraphicsContext::DrawGDIOnBitmap(const RectF& rect, std::function<void(GDIContext& dc)> func, bool forceAlpha)
	{
		if (!rect.IsEmpty())
		{
			// Draw GDI content on a memory DC to get a bitmap
			GDIBitmap bitmap(rect.GetSize(), ColorDepthDB::BPP32);
			{
				GDIMemoryContext dc(bitmap);

				// Set font
				if (auto font = GetFont())
				{
					dc.SetFont(font->ToFont());
				}

				// Set brush (solid only for now)
				if (auto brush = GetBrush())
				{
					dc.SetBrush(brush->GetColor());
				}

				// Set pen (solid only for now)
				if (auto pen = GetPen())
				{
					GDIPen penGDI(pen->GetColor(), pen->GetWidth());
					penGDI.SetSolid();
					penGDI.SetJoin(pen->GetLineJoin());
					penGDI.SetCap(pen->GetLineCap());

					dc.SetPen(penGDI);
				}

				// Call the actual drawing routine
				dc.ResetBoundingBox();
				std::invoke(func, dc);

				// Avoid drawing this bitmap on the context entirely if nothing has been drawn on the DC
				if (dc.GetBoundingBox().IsEmpty())
				{
					return {};
				}
			}

			// The context implementation can use this bitmap to draw it any way it need
			if (forceAlpha)
			{
				bitmap.ForceAlpha();
			}
			return bitmap;
		}
		return {};
	}
	String IGraphicsContext::EllipsizeTextGeneric(const String& text, float maxWidth, EllipsizeMode mode, FlagSet<EllipsizeFlag> flags, const IGraphicsFont& font) const
	{
		if (!text.IsEmpty() && maxWidth > 0)
		{
			if (mode == EllipsizeMode::None || GetTextExtent(text, font).GetWidth() <= maxWidth)
			{
				return text;
			}

			String result;

			// These cannot be cached between different 'Ellipsize' calls as they can
			// change because of e.g. a font change; however we calculate them only once
			// when ellipsizing multiline labels.
			const float replacementWidth = GetTextExtent(g_EllipseReplacement, font).GetWidth();

			// We must handle correctly labels with newlines
			String currentLine;
			for (auto it = text.begin(); ; ++it)
			{
				if (it == text.end() || *it == '\n')
				{
					currentLine.TrimLeft();
					currentLine = DoEllipsizeSingleLine(currentLine, *this, maxWidth, mode, flags, font, replacementWidth);

					// Add this (ellipsized) row to the rest of the label
					result += currentLine;
					if (it == text.end())
					{
						break;
					}

					result += *it;
					currentLine.clear();
				}
				else if (flags.Contains(EllipsizeFlag::ExpandTabs) && *it == '\t')
				{
					// We need also to expand tabs to properly calc their size.
					// Windows natively expands the TABs to 6 spaces. Do the same.
					currentLine += kxfS("      ");
				}
				else
				{
					currentLine += *it;
				}
			}
			return result;
		}
		return {};
	}

	// Offset management
	void IGraphicsContext::OffsetForScrollableArea(const PointF& scrollPos, const PointF& scrollInc, const PointF& scale, const PointF& origin)
	{
		TransformTranslate(origin.GetX() - scrollPos.GetX() * scrollInc.GetX(), origin.GetY() - scrollPos.GetY() * scrollInc.GetY());
		TransformScale(scale.GetX(), scale.GetY());
	}
	void IGraphicsContext::OffsetForScrollableArea(const wxScrollHelper& scrollableWidget)
	{
		const PointF scale(scrollableWidget.GetScaleX(), scrollableWidget.GetScaleY());
		const PointF scrollPos = Point(scrollableWidget.GetViewStart());

		Point scrollInc;
		scrollableWidget.GetScrollPixelsPerUnit(&scrollInc.X(), &scrollInc.Y());

		OffsetForScrollableArea(scrollPos, scrollInc, scale);
	}
}
