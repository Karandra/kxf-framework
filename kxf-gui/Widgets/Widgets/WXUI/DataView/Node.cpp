#include "kxf-pch.h"
#include "Node.h"
#include "View.h"
#include "MainWindow.h"

namespace kxf::DataView
{
	void RootNode::Initalize(WXUI::DataView::MainWindow& mainWindow) noexcept
	{
		m_Children.clear();

		m_DataModel = mainWindow.m_Model.get();
		m_MainWindow = &mainWindow;
		m_View = mainWindow.m_View;
	}
}

namespace kxf::DataView
{
	bool NodeOperation::DoWalk(const Node& node, NodeOperation& func)
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

		for (auto& childNode: node.m_Children)
		{
			if (DoWalk(childNode, func))
			{
				return true;
			}
		}
		return false;
	}
}

namespace kxf::DataView
{
	NodeOperation::Result RowToNodeOperation::operator()(const Node& node)
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
		else if (const size_t childrenCount = node.GetChildrenCount(); childrenCount != 0)
		{
			// If the current node has only leaf children, we can find the desired node directly.
			// This can speed up finding the node in some cases, and will have a very good effect for list views.
			if (node.GetSubTreeCount() == childrenCount)
			{
				const size_t index = m_Row - m_CurrentRow - 1;
				if (index < childrenCount)
				{
					m_ResultNode = &node.m_Children[index];
					return Result::Done;
				}
			}
		}
		return Result::Continue;
	}
}
