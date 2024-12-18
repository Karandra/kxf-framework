#include "kxf-pch.h"
#include "Node.h"
#include "SortMode.h"
#include "CellAttributes.h"
#include "../../Widgets/WXUI/DataView/Node.h"
#include "../../Widgets/WXUI/DataView/View.h"
#include "../../Widgets/WXUI/DataView/MainWindow.h"
#include "../../Widgets/WXUI/DataView/HeaderCtrl.h"

namespace kxf::DataView
{
	size_t Node::CalcSubTreeIndex() const
	{
		if (m_ParentNode)
		{
			size_t index = 0;
			for (const Node& node: m_ParentNode->m_Children)
			{
				if (node.IsSameAs(*this))
				{
					return index;
				}
				else
				{
					index++;
				}
			}
		}
		return npos;
	}
	size_t Node::CalcSubTreeCount(bool force) const
	{
		// Total count of expanded (i.e. visible with the help of some scrolling) items
		// in the subtree, but excluding this node. I.e. it is 0 for leaves and is the
		// number of rows the subtree occupies for branch nodes.

		size_t count = 0;
		if (m_IsExpanded || force || IsRootNode())
		{
			for (const Node& node: m_Children)
			{
				count += node.GetSubTreeCount() + 1;
			}
		}
		return count;
	}
	void Node::ChangeSubTreeCount(intptr_t num, bool force)
	{
		if (m_IsExpanded || force)
		{
			if (m_SubTreeCount != npos)
			{
				m_SubTreeCount += num;
				if (m_ParentNode)
				{
					m_ParentNode->ChangeSubTreeCount(num, force);
				}
			}
			else
			{
				m_SubTreeCount = CalcSubTreeCount(force);
				if (m_ParentNode)
				{
					m_ParentNode->m_SubTreeCount = m_ParentNode->CalcSubTreeCount(force);
				}
			}
		}
	}

	void Node::RefreshChildren()
	{
		if (IsAttached())
		{
			auto DoEnumerate = [&](auto& ref)
			{
				for (auto& node: m_Children)
				{
					node.OnDetach();
				}
				m_Children.clear();

				const size_t count = ref.GetChildrenCount();
				if (count != IDataViewItem::npos)
				{
					m_Children.reserve(count);
				}

				for (size_t i = 0; i < count; i++)
				{
					auto& node = m_Children.emplace_back(ref.GetChildItem(i), this);
					node.m_SubTreeIndex = i;

					if (node.IsNull())
					{
						m_Children.pop_back();
						break;
					}
				}

				for (auto& node: m_Children)
				{
					node.OnAttach();
				}
			};

			if (IsRootNode())
			{
				DoEnumerate(GetDataModel());
			}
			else if (m_Item)
			{
				DoEnumerate(*m_Item);
			}
		}
		else
		{
			m_Children.clear();
		}
		m_SubTreeCount = npos;
	}
	void Node::SortChildren(const DataView::SortMode& sortMode)
	{
		if (!m_Children.empty() && sortMode.GetSortOrder() != SortOrder::None)
		{
			DoSortChildren(sortMode);

			// Sort the subtree
			auto CollectNodes = [](std::vector<Node*>& subNodes, auto& children)
			{
				subNodes.reserve(children.size());
				for (auto& node: children)
				{
					if (!node.m_Children.empty())
					{
						subNodes.emplace_back(&node);
					}
				}
			};

			std::vector<Node*> nodes;
			CollectNodes(nodes, m_Children);

			while (!nodes.empty())
			{
				std::vector<Node*> subNodes;
				for (auto& node: nodes)
				{
					if (node->IsExpanded())
					{
						node->DoSortChildren(sortMode);
						CollectNodes(subNodes, node->m_Children);
					}
				}

				nodes = std::move(subNodes);
			}
		}
	}
	void Node::DoSortChildren(const DataView::SortMode& sortMode)
	{
		OnSortChildren(sortMode);

		std::sort(m_Children.begin(), m_Children.end(), [&](const Node& left, const Node& right) -> bool
		{
			switch (sortMode.GetSortOrder())
			{
				case SortOrder::Ascending:
				{
					return *left.m_Item < *right.m_Item;
				}
				case SortOrder::Descending:
				{
					return *left.m_Item > *right.m_Item;
				}
			};
			return false;
		});

		// Recalculate subtree indices
		size_t index = 0;
		for (auto& node: m_Children)
		{
			node.m_SubTreeIndex = index++;

			// Update parent node pointer
			for (auto& subNode: node.m_Children)
			{
				subNode.m_ParentNode = &node;
			}
		}
	}

	size_t Node::ToggleNodeExpandState()
	{
		// We do not allow the (invisible) root node to be collapsed because there is no way to expand it again.
		if (!IsRootNode())
		{
			// If we're expanding the node, but it has no children enumerated while it the item indicates that it should,
			// refresh them now.
			bool refreshed = false;
			if (!m_IsExpanded && m_Children.empty() && HasChildren())
			{
				RefreshChildren();
				refreshed = true;
			}

			auto count = static_cast<intptr_t>(CalcSubTreeCount(true));
			if (m_IsExpanded)
			{
				m_IsExpanded = false;
				ChangeSubTreeCount(-count, true);

				// If the entire subtree is collapsed, we can purge it to free the memory.
				if (m_SubTreeCount == 0)
				{
					size_t subTreeCount = 0;
					for (const auto& node: m_Children)
					{
						subTreeCount += node.GetSubTreeCount();
					}
					if (subTreeCount == 0)
					{
						m_Children.clear();
					}
				}
			}
			else
			{
				// If we've just refreshed this node's children, its initial subtree count is always 0,
				// let's account for that and avoid redundant calculations here.
				if (refreshed)
				{
					m_SubTreeCount = 0;
				}

				m_IsExpanded = true;
				ChangeSubTreeCount(+count, true);

				// Sort the children if needed
				SortChildren(GetView().GetSortMode());
			}
		}
		return m_SubTreeCount;
	}
	void Node::DoExpandNodeAncestors()
	{
		Node* node = m_ParentNode;
		while (node)
		{
			node->ExpandNode();
			node = node->m_ParentNode;
		}
	}
	void Node::DoEnsureCellVisible(const Column* column)
	{
		DoExpandNodeAncestors();

		if (Row row = GetRow())
		{
			auto& mainWindow = GetMainWindow();
			mainWindow.RecalculateDisplay();

			if (column)
			{
				mainWindow.EnsureVisible(row, column->GetIndex());
			}
			else
			{
				mainWindow.EnsureVisible(row);
			}
		}
	}

	Rect Node::DoGetCellRect(const Column* column) const
	{
		Rect rect = DoGetCellClientRect(column);
		if (auto header = GetView().GetHeaderCtrl())
		{
			rect.SetTop(rect.GetTop() + header->GetSize().GetHeight());
		}
		return rect;
	}
	Rect Node::DoGetCellClientRect(const Column* column) const
	{
		return GetMainWindow().GetItemRect(*this, column);
	}
	Point Node::DoGetCellDropdownPoint(const Column* column) const
	{
		return DoGetCellRect(column).GetLeftBottom() + Point(GetView().FromDIP(Point(0, 1)));
	}

	WXUI::DataView::View& Node::GetView() const
	{
		return *m_RootNode->m_View;
	}
	WXUI::DataView::MainWindow& Node::GetMainWindow() const
	{
		return *m_RootNode->m_MainWindow;
	}
	IDataViewModel& Node::GetDataModel() const
	{
		return *m_RootNode->m_DataModel;
	}

	bool Node::IsRootNode() const noexcept
	{
		return this == m_RootNode;
	}
	bool Node::IsAttached() const noexcept
	{
		return m_RootNode && m_RootNode->m_DataModel;
	}
	void Node::ExpandNode()
	{
		DoExpandNodeAncestors();
		GetMainWindow().Expand(*this);
	}
	void Node::CollapseNode()
	{
		GetMainWindow().Collapse(*this);
	}

	void Node::RefreshCell()
	{
		GetMainWindow().OnCellChanged(*this, nullptr);
	}
	void Node::RefreshCell(Column& column)
	{
		GetMainWindow().OnCellChanged(*this, &column);
	}
	bool Node::EditCell(Column& column)
	{
		return GetMainWindow().BeginEdit(*this, column);
	}

	Row Node::GetRow() const
	{
		return GetMainWindow().GetRowByNode(*this);
	}
	int Node::GetIndentLevel() const
	{
		if (IsRootNode())
		{
			return -1;
		}
		else
		{
			int level = 0;

			const Node* node = this;
			while (node->m_ParentNode && node->m_ParentNode->m_ParentNode)
			{
				node = node->m_ParentNode;
				level++;
			}
			return level;
		}
	}

	CellState Node::GetCellState() const
	{
		return GetMainWindow().GetCellStateForRow(GetRow());
	}
	void Node::SelectItem()
	{
		DoExpandNodeAncestors();
		if (Row row = GetRow())
		{
			auto& mainWindow = GetMainWindow();

			// Unselect all rows before select another in the single select mode
			if (mainWindow.IsSingleSelection())
			{
				mainWindow.UnselectAllRows();
			}

			// Select the row and set focus to the selected item
			mainWindow.SelectRow(row);
			mainWindow.ChangeCurrentRow(row);
		}
	}
	void Node::UnselectItem()
	{
		GetMainWindow().SelectRow(GetRow(), false);
	}
	void Node::MakeCurrent()
	{
		auto& mainWindow = GetMainWindow();
		if (mainWindow.IsMultipleSelection())
		{
			Row newCurrent = GetRow();
			Row oldCurrent = mainWindow.GetCurrentRow();

			if (newCurrent != oldCurrent)
			{
				mainWindow.ChangeCurrentRow(newCurrent);
				mainWindow.RefreshRow(oldCurrent);
				mainWindow.RefreshRow(newCurrent);
			}
		}
		else
		{
			SelectItem();
		}
	}
}
