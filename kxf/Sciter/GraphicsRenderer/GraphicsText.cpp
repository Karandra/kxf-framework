#include "kxf-pch.h"
#include "GraphicsText.h"
#include "kxf/Sciter/ScriptValue.h"
#include "kxf/Sciter/SciterAPI.h"
#include "kxf/Sciter/Element.h"
#include "kxf/Sciter/Private/Conversion.h"

namespace kxf::Sciter
{
	bool GraphicsText::DoAcquire(GraphicsTextHandle* handle)
	{
		return GetGrapchicsAPI()->textAddRef(ToSciterText(handle)) == GRAPHIN_OK;
	}
	void GraphicsText::DoRelease()
	{
		GetGrapchicsAPI()->textRelease(ToSciterText(m_Handle));
	}

	GraphicsText::GraphicsText(const ScriptValue& value)
	{
		HTEXT handle = nullptr;
		if (GetGrapchicsAPI()->vUnWrapText(ToSciterScriptValue(value.GetNativeValue()), &handle))
		{
			Acquire(FromSciterText(handle));
		}
	}

	ScriptValue GraphicsText::ToScriptValue() const
	{
		ScriptValue value;
		if (GetGrapchicsAPI()->vWrapText(ToSciterText(m_Handle), ToSciterScriptValue(value.GetNativeValue())) == GRAPHIN_OK)
		{
			return value;
		}
		return {};
	}

	bool GraphicsText::CreateForElement(const String& text, const Element& element, const String& className)
	{
		HTEXT handle = nullptr;
		if (GetGrapchicsAPI()->textCreateForElement(&handle, text.wc_str(), text.length(), ToSciterElement(element.GetHandle()), className.IsEmpty() ? nullptr : className.wc_str()) == GRAPHIN_OK)
		{
			Acquire(FromSciterText(handle));
			return true;
		}
		return false;
	}
	bool GraphicsText::CreateForElementAndStyle(const String& text, const Element& element, const String& style)
	{
		const XChar* stylePtr = style.IsEmpty() ? nullptr : style.wc_str();
		size_t styleLength = style.IsEmpty() ? 0 : style.length();

		HTEXT handle = nullptr;
		if (GetGrapchicsAPI()->textCreateForElementAndStyle(&handle, text.wc_str(), text.length(), ToSciterElement(element.GetHandle()), stylePtr, styleLength) == GRAPHIN_OK)
		{
			Acquire(FromSciterText(handle));
			return true;
		}
		return false;
	}
	
	std::optional<TextMetrics> GraphicsText::GetMetrics() const
	{
		float minWidth = 0;
		float maxWidth = 0;
		float height = 0;
		float ascent = 0;
		float descent = 0;
		UINT lineCount = 0;

		if (GetGrapchicsAPI()->textGetMetrics(ToSciterText(m_Handle), &minWidth, &maxWidth, &height, &ascent, &descent, &lineCount) == GRAPHIN_OK)
		{
			TextMetrics metrics;
			metrics.MinWidth = minWidth;
			metrics.MaxWidth = maxWidth;
			metrics.Height = height;
			metrics.Ascent = ascent;
			metrics.Descent = descent;
			metrics.LineCount = lineCount;
			
			return metrics;
		}
		return std::nullopt;
	}
	bool GraphicsText::SetBox(double width, double height)
	{
		return GetGrapchicsAPI()->textSetBox(ToSciterText(m_Handle), width, height) == GRAPHIN_OK;
	}
}
