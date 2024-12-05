#pragma once
#include "Common.h"
#include "CellState.h"
#include "CellAttributes.h"
#include "ToolTip.h"
#include "kxf/Core/Any.h"

namespace kxf
{
	class IGraphicsContext;
	class IGraphicsRenderer;
	class WidgetMouseEvent;
}
namespace kxf::DataView
{
	class CellRenderer;
}
namespace kxf::WXUI::DataView
{
	class View;
	class MainWindow;
	class MaxWidthCalculator;
}

namespace kxf
{
	class KX_API IDataViewCellRenderer: public RTTI::Interface<IDataViewCellRenderer>
	{
		KxRTTI_DeclareIID(IDataViewCellRenderer, {0xa9538cec, 0x83d5, 0x46b9, {0x9a, 0x9c, 0x1, 0x49, 0x86, 0x67, 0x21, 0x5e}});

		friend class DataView::CellRenderer;

		public:
			enum class MarkupMode
			{
				Disabled = 0,
				TextOnly,
				WithMnemonics
			};

			struct RenderInfo final
			{
				const DataView::Node& Node;
				const DataView::Column& Column;

				DataView::CellState State;
				DataView::CellAttributes Attributes;
				EllipsizeMode EllipsizeMode = EllipsizeMode::End;
				MarkupMode MarkupMode = MarkupMode::Disabled;
				Rect CellRect;

				IGraphicsContext* GraphicsContext = nullptr;
				const WidgetMouseEvent* MouseEvent = nullptr;
			};

		protected:
			virtual void DrawBackground(const RenderInfo& renderInfo)
			{
			}
			virtual void DrawContent(const RenderInfo& renderInfo) = 0;

			virtual Any OnActivate(const RenderInfo& renderInfo)
			{
				return {};
			}
			virtual Size GetCellSize(const RenderInfo& renderInfo) const
			{
				return {0, 0};
			}

		public:
			virtual String GetDisplayText(const Any& value) const = 0;
			virtual bool SetDisplayValue(Any value) = 0;

			virtual DataView::ToolTip CreateToolTip() const
			{
				return {};
			}
			virtual FlagSet<Alignment> GetEffectiveAlignment(const RenderInfo& renderInfo, FlagSet<Alignment> alignment) const;
			virtual bool IsActivatable() const
			{
				return false;
			}
	};
}

namespace kxf::DataView
{
	class KX_API CellRenderer final
	{
		friend class WXUI::DataView::MainWindow;
		friend class WXUI::DataView::MaxWidthCalculator;

		private:
			using MarkupMode = IDataViewCellRenderer::MarkupMode;
			struct RenderParameters final
			{
				DataView::CellState State;
				DataView::CellAttributes Attributes;
				EllipsizeMode EllipsizeMode = EllipsizeMode::End;
				MarkupMode MarkupMode = MarkupMode::Disabled;
				Rect CellRect;
			};

		private:
			std::shared_ptr<IDataViewCellRenderer> m_CellRenderer;

			RenderParameters m_Parameters;
			IGraphicsContext* m_GraphicsContext = nullptr;
			FlagSet<Alignment> m_Alignment = Alignment::Invalid;
			const Node* m_Node = nullptr;
			const Column* m_Column = nullptr;
			bool m_IsViewEnabled = false;
			bool m_IsViewFocused = false;

		private:
			bool IsNull() const noexcept
			{
				return m_CellRenderer == nullptr;
			}
			bool CanDraw() const noexcept
			{
				return m_CellRenderer && m_GraphicsContext != nullptr;
			}
			IDataViewCellRenderer::RenderInfo CreateParemeters() const
			{
				IDataViewCellRenderer::RenderInfo info = {*m_Node, *m_Column};
				info.State = m_Parameters.State;
				info.Attributes = m_Parameters.Attributes;
				info.EllipsizeMode = m_Parameters.EllipsizeMode;
				info.MarkupMode = m_Parameters.MarkupMode;
				info.CellRect = m_Parameters.CellRect;
				info.GraphicsContext = m_GraphicsContext;

				return info;
			}

			bool BeginCellRendering(const Node& node, const Column& column, IGraphicsContext& gc)
			{
				m_Node = &node;
				m_Column = &column;
				m_GraphicsContext = &gc;

				return CanDraw();
			}
			void EndCellRendering()
			{
				EndCellSetup();
			}

			void BeginCellSetup(const Node& node, const Column& column, IGraphicsContext* gc = nullptr)
			{
				EndCellSetup();

				m_Node = &node;
				m_Column = &column;
				m_GraphicsContext = gc;
			}
			void EndCellSetup()
			{
				m_Parameters = {};
				m_GraphicsContext = nullptr;
				m_Alignment = Alignment::Invalid;
				m_Node = nullptr;
				m_Column = nullptr;
				m_IsViewEnabled = false;
				m_IsViewFocused = false;
			}

			void SetupCellDisplayValue();
			const CellAttributes& SetupCellAttributes(CellState cellState);

			void DrawCellBackground(const Rect& cellRect, CellState cellState, bool noUserBackground = false);
			std::pair<Size, Rect> DrawCellContent(const Rect& cellRect, CellState cellState);
			void OnActivateCell(Node& node, const Rect& cellRect, const WidgetMouseEvent* mouseEvent = nullptr);

			const CellAttributes& GetAttributes() const
			{
				return m_Parameters.Attributes;
			}
			IGraphicsContext& GetGraphicsContext() const
			{
				return *m_GraphicsContext;
			}

		public:
			CellRenderer(std::shared_ptr<IDataViewCellRenderer> cellRenerer = nullptr)
				:m_CellRenderer(std::move(cellRenerer))
			{
			}
			CellRenderer(const CellRenderer&) = delete;
			CellRenderer(CellRenderer&& other) noexcept
			{
				*this = std::move(other);
			}
			~CellRenderer() = default;

		public:
			IDataViewWidget& GetOwningWidget() const;
			const Column& GetColumn() const
			{
				return *m_Column;
			}
			const Node& GetNode() const
			{
				return *m_Node;
			}

			bool IsActivatable() const
			{
				return m_CellRenderer && m_CellRenderer->IsActivatable();
			}

			void SetAlignment(FlagSet<Alignment> alignment)
			{
				m_Alignment = alignment;
			}
			void SetEllipsizeMode(EllipsizeMode mode)
			{
				m_Parameters.EllipsizeMode = mode;
			}

			bool IsMarkupEnabled() const
			{
				return m_Parameters.MarkupMode != MarkupMode::Disabled;
			}
			void EnableMarkup()
			{
				m_Parameters.MarkupMode = IDataViewCellRenderer::MarkupMode::TextOnly;
			}
			void EnableMarkupWithMnemonics()
			{
				m_Parameters.MarkupMode = IDataViewCellRenderer::MarkupMode::WithMnemonics;
			}
			void DisableMarkup()
			{
				m_Parameters.MarkupMode = IDataViewCellRenderer::MarkupMode::Disabled;
			}
			String StripMarkup(const String& markup) const;

			FlagSet<Alignment> GetEffectiveAlignment() const;
			Size GetEffectiveCellSize() const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			CellRenderer& operator=(const CellRenderer&) = delete;
			CellRenderer& operator=(CellRenderer&& other) noexcept
			{
				m_CellRenderer = std::move(other.m_CellRenderer);

				m_Parameters = std::move(other.m_Parameters);
				m_GraphicsContext = std::exchange(other.m_GraphicsContext, nullptr);
				m_Alignment = other.m_Alignment;
				m_Node = std::exchange(other.m_Node, nullptr);
				m_Column = std::exchange(other.m_Column, nullptr);
				m_IsViewEnabled = other.m_IsViewEnabled;
				m_IsViewFocused = other.m_IsViewFocused;

				return *this;
			}
	};
}
