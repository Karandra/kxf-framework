#include "kxf-pch.h"
#include "Node.h"
#include "View.h"
#include "MainWindow.h"
#include "HeaderCtrl.h"
#include "SortMode.h"

namespace kxf::UI::DataView
{
	size_t Node::ToggleNodeExpandState()
	{
		// We do not allow the (invisible) root node to be collapsed because there is no way to expand it again.
		if (!IsRootNode())
		{
			intptr_t count = 0;
			EnumChildren([&](const Node& node)
			{
				count += static_cast<intptr_t>(node.GetSubTreeCount()) + 1;
				return true;
			});

			if (m_IsExpanded)
			{
				ChangeSubTreeCount(-count);
				m_IsExpanded = false;
			}
			else
			{
				m_IsExpanded = true;
				ChangeSubTreeCount(+count);

				// Sort the children if needed
				OnSortChildren(GetView().GetSortMode());
			}
		}
		return m_SubTreeCount;

		#if 0
		m_IsExpanded = !m_IsExpanded;
		OnSortChildren();

		return GetSubTreeCount();
		#endif
	}
	void Node::RecalcSubTreeCount()
	{
		m_SubTreeCount = GetSubTreeCount();
	}
	void Node::ChangeSubTreeCount(intptr_t num)
	{
		if (m_IsExpanded)
		{
			if (m_SubTreeCount != std::numeric_limits<size_t>::max())
			{
				m_SubTreeCount += num;
				if (m_ParentNode)
				{
					m_ParentNode->ChangeSubTreeCount(num);
				}
			}
			else
			{
				RecalcSubTreeCount();
				if (m_ParentNode)
				{
					m_ParentNode->RecalcSubTreeCount();
				}
			}
		}
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

		if (Row row = GetItemRow())
		{
			MainWindow& mainWindow = GetMainWindow();
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
		return DoGetCellRect(column).GetLeftBottom() + Point(GetMainWindow().FromDIP(Point(0, 1)));
	}

	void Node::CreateNode(Node& parent)
	{
		m_ParentNode = &parent;
		m_RootNode = &parent.GetRootNode();
	}

	bool Node::IsRootNode() const
	{
		return this == m_RootNode && m_ParentNode == nullptr;
	}
	bool Node::IsNodeAttached() const
	{
		if (m_RootNode)
		{
			return m_RootNode->IsNodeAttached();
		}
		return false;
	}

	size_t Node::GetSubTreeCount() const
	{
		// Total count of expanded (i.e. visible with the help of some scrolling) items
		// in the subtree, but excluding this node. I.e. it is 0 for leaves and is the
		// number of rows the subtree occupies for branch nodes.

		if (m_SubTreeCount == std::numeric_limits<size_t>::max())
		{
			size_t count = 0;
			if (m_IsExpanded || IsRootNode())
			{
				count += EnumChildren([&](const Node& node)
				{
					count += node.GetSubTreeCount();
					return true;
				});
			}
			m_SubTreeCount = count;
		}
		return m_SubTreeCount;
	}
	size_t Node::GetSubTreeIndex() const
	{
		if (m_ParentNode)
		{
			size_t index = 0;
			size_t count = m_ParentNode->EnumChildren([&](const Node& node)
			{
				if (&node == this)
				{
					return false;
				}
				else
				{
					index++;
					return true;
				}
			});

			if (index < count)
			{
				return index;
			}
		}
		return std::numeric_limits<size_t>::max();
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

	View& Node::GetView() const
	{
		return m_RootNode->GetView();
	}
	Model& Node::GetModel() const
	{
		return *m_RootNode->GetView().GetModel();
	}
	MainWindow& Node::GetMainWindow() const
	{
		return *m_RootNode->GetView().GetMainWindow();
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

	Row Node::GetItemRow() const
	{
		return GetMainWindow().GetRowByNode(*this);
	}
	int Node::GetItemIndent() const
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
		return GetMainWindow().GetCellStateForRow(GetItemRow());
	}
	void Node::SelectItem()
	{
		DoExpandNodeAncestors();
		if (Row row = GetItemRow())
		{
			MainWindow& mainWindow = GetMainWindow();

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
		GetMainWindow().SelectRow(GetItemRow(), false);
	}
	void Node::MakeItemCurrent()
	{
		MainWindow& mainWindow = GetMainWindow();
		if (mainWindow.IsMultipleSelection())
		{
			const size_t newCurrent = *GetItemRow();
			const size_t oldCurrent = *mainWindow.GetCurrentRow();

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

namespace kxf::UI::DataView
{
	bool Node::IsCellEditable(const Column& column) const
	{
		Model& model = GetModel();
		return model.IsEnabled(*this, column) && model.GetEditor(*this, column) != nullptr;
	}
	bool Node::IsCellRenderable(const Column& column) const
	{
		Model& model = GetModel();
		MainWindow& mainWindow = GetMainWindow();

		return &model.GetRenderer(*this, column) != &mainWindow.GetNullRenderer();
	}
	bool Node::IsCellActivatable(const Column& column) const
	{
		Model& model = GetModel();
		return model.IsEnabled(*this, column) && model.GetRenderer(*this, column).IsActivatable();
	}

	Renderer& Node::GetCellRenderer(const Column& column) const
	{
		return GetModel().GetRenderer(*this, column);
	}
	Editor* Node::GetCellEditor(const Column& column) const
	{
		return GetModel().GetEditor(*this, column);
	}
	bool Node::IsCellEnabled(const Column& column) const
	{
		return GetModel().IsEnabled(*this, column);
	}

	Any Node::GetCellDisplayValue(const Column& column) const
	{
		return GetModel().GetDisplayValue(*this, column);
	}
	Any Node::GetCellValue(const Column& column) const
	{
		return GetModel().GetValue(*this, column);
	}
	ToolTip Node::GetCellToolTip(const Column& column) const
	{
		return GetModel().GetToolTip(*this, column);
	}
	bool Node::SetCellValue(Column& column, Any value)
	{
		return GetModel().SetValue(*this, column, std::move(value));
	}

	CellAttribute Node::GetCellAttributes(const Column& column, const CellState& cellState) const
	{
		return GetModel().GetAttributes(*this, column, cellState);
	}
	int Node::GetItemHeight() const
	{
		return GetModel().GetRowHeight(*this);
	}
}

namespace kxf::UI::DataView
{
	void RootNode::NotifyItemsChanged()
	{
		GetModel().NotifyItemsChanged();
	}
}

namespace kxf::UI::DataView
{
	bool NodeOperation::DoWalk(Node& node, NodeOperation& func)
	{
		switch (func(node))
		{
			case Result::Done:
			{
				return true;
			}
			case Result::SkipSubTree:
			{
				return false;
			}
			case Result::Continue:
			{
				break;
			}
		};

		bool result = false;
		node.EnumChildren([&](Node& childNode)
		{
			if (DoWalk(childNode, func))
			{
				result = true;
			}
			return !result;
		});
		return result;
	}
}

namespace kxf::UI::DataView
{
	NodeOperation::Result NodeOperation_RowToNode::operator()(Node& node)
	{
		m_CurrentRow++;
		if (m_CurrentRow == m_Row)
		{
			m_ResultNode = &node;
			return Result::Done;
		}

		if (node.GetSubTreeCount() + m_CurrentRow < static_cast<size_t>(m_Row))
		{
			m_CurrentRow += node.GetSubTreeCount();
			return Result::SkipSubTree;
		}
		return Result::Continue;
	}
}
