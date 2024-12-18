#include "kxf-pch.h"
#include "Model.h"
#include "MainWindow.h"

namespace kxf::UI::DataView
{
	void Model::DoOnModelAttached(MainWindow& mainWindow)
	{
		m_MainWindow = &mainWindow;
		m_View = mainWindow.GetView();

		OnModelAttached();
		GetRootNode().OnNodeAttached(*mainWindow.GetView());
	}
	void Model::DoOnModelDetached()
	{
		m_View = nullptr;
		m_MainWindow = nullptr;

		OnModelDetached();
		GetRootNode().OnNodeDetached();
	}

	void Model::NotifyItemsChanged()
	{
		if (m_MainWindow)
		{
			m_MainWindow->ItemsChanged();
		}
	}

	Renderer& Model::GetRenderer(const Node& node, const Column& column) const
	{
		return column.GetRenderer();
	}
	Editor* Model::GetEditor(const Node& node, const Column& column) const
	{
		return column.GetEditor();
	}
	ToolTip Model::GetToolTip(const Node& node, const Column& column) const
	{
		return GetRenderer(node, column).CreateToolTip();
	}
}
