#pragma once
#include "Common.h"
#include "../../DataView/Node.h"

namespace kxf::DataView
{
	class KXF_API RootNode final: public Node
	{
		kxf_RTTI_DeclareIID(RootNode, {0xe57d0d6b, 0xdea1, 0x43d9, {0xb3, 0xf2, 0x75, 0xd1, 0xce, 0xc2, 0x32, 0x59}});
		
		friend class Node;
		friend class WXUI::DataView::View;
		friend class WXUI::DataView::MainWindow;

		private:
			IDataViewModel* m_DataModel = nullptr;
			WXUI::DataView::View* m_View = nullptr;
			WXUI::DataView::MainWindow* m_MainWindow = nullptr;

		private:

		public:
			RootNode() noexcept
			{
				m_RootNode = this;
				m_ParentNode = nullptr;
				m_IsExpanded = true;
			}

		public:
			void Initalize(WXUI::DataView::MainWindow& mainWindow) noexcept;
	};
}

namespace kxf::DataView
{
	class KXF_API NodeOperation
	{
		protected:
			// The return value control how the tree-walker traverses the tree
			enum class Result
			{
				Done, // Done, stop traversing and return
				SkipSubTree, // Ignore the current node's subtree and continue
				Continue, // Not done, continue
			};

		private:
			static bool DoWalk(const Node& node, NodeOperation& func);

		protected:
			virtual Result operator()(const Node& node) = 0;

		public:
			virtual ~NodeOperation() = default;

		public:
			bool Walk(const Node& node)
			{
				return DoWalk(node, *this);
			}
	};
}

namespace kxf::DataView
{
	class KXF_API RowToNodeOperation final: public NodeOperation
	{
		private:
			const intptr_t m_Row = -1;
			intptr_t m_CurrentRow = -1;
			const Node* m_ResultNode = nullptr;

		public:
			RowToNodeOperation(intptr_t row , intptr_t current = -2)
				:m_Row(row), m_CurrentRow(current)
			{
			}

		public:
			Result operator()(const Node& node) override;

			const Node* GetResult() const
			{
				return m_ResultNode;
			}
	};
}
