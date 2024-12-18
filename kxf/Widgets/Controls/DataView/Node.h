#pragma once
#include "Common.h"
#include "Row.h"
#include "Column.h"
#include "ToolTip.h"
#include "kxf/Core/Any.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::UI::DataView
{
	class CellAttribute;
	class MainWindow;
	class Renderer;
	class Editor;
	class Model;
	class View;
	class Node;
	class SortMode;
	class CellState;
}

namespace kxf::UI::DataView
{
	class RootNode;

	class KXF_API Node: public RTTI::Interface<Node>
	{
		kxf_RTTI_DeclareIID(Node, {0x84870de7, 0x1623, 0x4b27, {0x9a, 0x8b, 0x79, 0x26, 0x81, 0x9d, 0xbf, 0x28}});

		friend class MainWindow;
		friend class View;
		friend class Model;
		friend class RootNode;

		private:
			RootNode* m_RootNode = nullptr;
			Node* m_ParentNode = nullptr;

			mutable size_t m_SubTreeCount = std::numeric_limits<size_t>::max();
			bool m_IsExpanded = false;

		private:
			size_t ToggleNodeExpandState();
			void RecalcSubTreeCount();
			void ChangeSubTreeCount(intptr_t num);

			void DoExpandNodeAncestors();
			void DoEnsureCellVisible(const Column* column);
			Rect DoGetCellRect(const Column* column) const;
			Rect DoGetCellClientRect(const Column* column) const;
			Point DoGetCellDropdownPoint(const Column* column) const;

		protected:
			virtual void OnSortChildren(const SortMode& sortMode)
			{
			}
			virtual size_t OnEnumChildren(std::function<bool(Node&)> func)
			{
				return 0;
			}

			virtual bool OnExpandNode()
			{
				return true;
			}
			virtual bool OnCollapseNode()
			{
				return true;
			}

			void CreateNode(Node& parent);

		public:
			Node() = default;
			Node(const Node&) = default;
			Node(Node* parent)
			{
				CreateNode(*parent);
			}
			virtual ~Node() = default;

		public:
			bool IsRootNode() const;
			bool IsNodeAttached() const;

			RootNode& GetRootNode() const
			{
				return *m_RootNode;
			}
			Node* GetParentNode() const
			{
				return m_ParentNode;
			}
			bool HasParentNode() const
			{
				return m_ParentNode != nullptr;
			}

			size_t EnumChildren(std::function<bool(Node&)> func)
			{
				return OnEnumChildren(std::move(func));
			}
			size_t EnumChildren(std::function<bool(const Node&)> func) const
			{
				if (func)
				{
					return const_cast<Node&>(*this).OnEnumChildren([&](Node& node)
					{
						return std::invoke(func, node);
					});
				}
				else
				{
					return const_cast<Node&>(*this).OnEnumChildren({});
				}
			}

			bool HasChildren() const
			{
				return GetChildrenCount() != 0;
			}
			size_t GetChildrenCount() const
			{
				return EnumChildren({});
			}
			size_t GetSubTreeCount() const;
			size_t GetSubTreeIndex() const;

			bool IsNodeExpanded() const
			{
				return m_IsExpanded;
			}
			void ExpandNode();
			void CollapseNode();

		public:
			View& GetView() const;
			Model& GetModel() const;
			MainWindow& GetMainWindow() const;

			void RefreshCell();
			void RefreshCell(Column& column);
			bool EditCell(Column& column);

			Row GetItemRow() const;
			int GetItemIndent() const;

			CellState GetCellState() const;
			void SelectItem();
			void UnselectItem();
			void MakeItemCurrent();

			void EnsureCellVisible()
			{
				DoEnsureCellVisible(nullptr);
			}
			void EnsureCellVisible(const Column& column)
			{
				DoEnsureCellVisible(&column);
			}

			Rect GetCellRect() const
			{
				return DoGetCellRect(nullptr);
			}
			Rect GetCellRect(const Column& column) const
			{
				return DoGetCellRect(&column);
			}

			Rect GetCellClientRect() const
			{
				return DoGetCellClientRect(nullptr);
			}
			Rect GetCellClientRect(const Column& column) const
			{
				return DoGetCellClientRect(&column);
			}

			Point GetCellDropdownPoint() const
			{
				return DoGetCellDropdownPoint(nullptr);
			}
			Point GetCellDropdownPoint(const Column& column) const
			{
				return DoGetCellDropdownPoint(&column);
			}

		public:
			bool IsCellEditable(const Column& column) const;
			bool IsCellRenderable(const Column& column) const;
			bool IsCellActivatable(const Column& column) const;

			virtual Renderer& GetCellRenderer(const Column& column) const;
			virtual Editor* GetCellEditor(const Column& column) const;
			virtual bool IsCellEnabled(const Column& column) const;

			virtual ToolTip GetCellToolTip(const Column& column) const;
			virtual Any GetCellDisplayValue(const Column& column) const;
			virtual Any GetCellValue(const Column& column) const;
			virtual bool SetCellValue(Column& column, Any value);

			virtual CellAttribute GetCellAttributes(const Column& column, const CellState& cellState) const;
			virtual int GetItemHeight() const;

		public:
			Node& operator=(const Node&) = default;
	};

	class KXF_API RootNode: public Node
	{
		kxf_RTTI_DeclareIID(RootNode, {0xe57d0d6b, 0xdea1, 0x43d9, {0xb3, 0xf2, 0x75, 0xd1, 0xce, 0xc2, 0x32, 0x59}});

		public:
			RootNode()
			{
				m_RootNode = this;
				m_ParentNode = nullptr;
				m_IsExpanded = true;
			}

		public:
			virtual View& GetView() const = 0;

			virtual void OnNodeAttached(View& view)
			{
			}
			virtual void OnNodeDetached()
			{
			}

		public:
			void NotifyItemsChanged();
	};
}

namespace kxf::UI::DataView
{
	class KXF_API NodeOperation
	{
		public:
			// The return value control how the tree-walker traverse the tree
			enum class Result
			{
				Done, // Done, stop traversing and return
				SkipSubTree, // Ignore the current node's subtree and continue
				Continue, // Not done, continue
			};

		private:
			static bool DoWalk(Node& node, NodeOperation& func);

		protected:
			virtual Result operator()(Node& node) = 0;

		public:
			virtual ~NodeOperation() = default;

		public:
			bool Walk(Node& node)
			{
				return DoWalk(node, *this);
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API NodeOperation_RowToNode final: public NodeOperation
	{
		private:
			const intptr_t m_Row = -1;
			intptr_t m_CurrentRow = -1;
			Node* m_ResultNode = nullptr;

		public:
			NodeOperation_RowToNode(intptr_t row , intptr_t current)
				:m_Row(row), m_CurrentRow(current)
			{
			}

		public:
			Result operator()(Node& node) override;

			Node* GetResult() const
			{
				return m_ResultNode;
			}
	};
}
