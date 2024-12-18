#pragma once
#include "Common.h"
#include "Utility/HandleWrapper.h"
#include <utility>

namespace kxf::Sciter
{
	class Element;
	struct NodeHandle;
}

namespace kxf::Sciter
{
	class KXF_API Node final: public HandleWrapper<Node, NodeHandle>
	{
		friend class HandleWrapper<Node, NodeHandle>;

		public:
			static Node CreateTextNode(const String& value);
			static Node CreateCommentNode(const String& value);

		private:
			NodeHandle* m_Handle = nullptr;

		private:
			bool DoAcquire(NodeHandle* handle) noexcept;
			void DoRelease() noexcept;

		public:
			Node() noexcept = default;
			Node(NodeHandle* handle) noexcept
				:HandleWrapper(handle)
			{
			}
			Node(const Node& other) noexcept
				:HandleWrapper(other)
			{
			}
			Node(Node&& other) noexcept
				:HandleWrapper(std::move(other))
			{
			}

		public:
			bool Detach();
			bool Remove();

			bool IsText() const;
			bool IsElement() const;
			bool IsComment() const;

			Element ToElement() const;

			Node GetParent() const;
			Node GetPrevSibling() const;
			Node GetNextSibling() const;
			Node GetFirstChild() const;
			Node GetLastChild() const;

			size_t GetIndexWithinParent() const;
			size_t GetChildrenCount() const;
			Node GetChildAt(size_t index) const;
			Node operator[](size_t index) const
			{
				return GetChildAt(index);
			}

			bool AppendChild(const Node& childNode);
			bool PrependChild(const Node& childNode);
			bool InsertChildBefore(const Node& childNode);
			bool InsertChildAfter(const Node& childNode);

			String GetValue() const;
			bool SetValue(StringView value) const;
			bool SetValue(const String& value) const
			{
				return SetValue(value.view());
			}

		public:
			Node& operator=(const Node& other) noexcept
			{
				CopyFrom(other);
				return *this;
			}
			Node& operator=(Node&& other) noexcept
			{
				MoveFrom(other);
				return *this;
			}
			Node& operator=(NodeHandle* handle) noexcept
			{
				CopyFrom(handle);
				return *this;
			}
	};
}
