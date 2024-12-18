#pragma once
#include "kxf/Sciter/Common.h"
#include "kxf/Sciter/Utility/HandleWrapper.h"

namespace kxf::Sciter
{
	struct GraphicsTextHandle;
	class ScriptValue;
	class Element;
}

namespace kxf::Sciter
{
	class KXF_API GraphicsText final: public HandleWrapper<GraphicsText, GraphicsTextHandle>
	{
		friend class HandleWrapper<GraphicsText, GraphicsTextHandle>;
		
		private:
			GraphicsTextHandle* m_Handle = nullptr;

		private:
			bool DoAcquire(GraphicsTextHandle* handle);
			void DoRelease();

		public:
			GraphicsText() = default;
			GraphicsText(GraphicsTextHandle* handle)
				:HandleWrapper(handle)
			{
			}
			GraphicsText(const GraphicsText& other)
				:HandleWrapper(other)
			{
			}
			GraphicsText(const ScriptValue& value);
			GraphicsText(GraphicsText&& other)
				:HandleWrapper(std::move(other))
			{
			}

		public:
			ScriptValue ToScriptValue() const;

			bool CreateForElement(const String& text, const Element& element, const String& className = {});
			bool CreateForElementAndStyle(const String& text, const Element& element, const String& style = {});
			
			std::optional<TextMetrics> GetMetrics() const;
			Size GetMinExtent() const
			{
				auto metrics = GetMetrics();
				return metrics ? Size(metrics->MinWidth, metrics->Height) : Size::UnspecifiedSize();
			}
			Size GetMaxExtent() const
			{
				auto metrics = GetMetrics();
				return metrics ? Size(metrics->MaxWidth, metrics->Height) : Size::UnspecifiedSize();
			}
			int GetLineCount() const
			{
				auto metrics = GetMetrics();
				return metrics ? metrics->LineCount : 0;
			}

			bool SetBox(double width, double height);
			bool SetBox(const Size& box)
			{
				return SetBox(box.GetWidth(), box.GetHeight());
			}

		public:
			GraphicsText& operator=(const GraphicsText& other)
			{
				CopyFrom(other);
				return *this;
			}
			GraphicsText& operator=(GraphicsText&& other)
			{
				MoveFrom(other);
				return *this;
			}
			GraphicsText& operator=(GraphicsTextHandle* handle)
			{
				CopyFrom(handle);
				return *this;
			}
	};
}
