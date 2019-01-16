#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithOptions.h"
#include "Common.h"
#include "CellState.h"
#include "ItemAttributes.h"
#include "RenderEngine.h"

namespace Kx::DataView2
{
	class KX_API Node;
	class KX_API View;
	class KX_API MainWindow;
	class KX_API Column;
	class KX_API RenderEngine;
}

namespace Kx::DataView2
{
	class KX_API Renderer
	{
		friend class Column;
		friend class MainWindow;
		friend class MaxWidthCalculator;
		friend class RenderEngine;

		public:
			enum class ProgressBarState
			{
				Normal,
				Paused,
				Error,
				Partial,
			};
			enum class MarkupMode
			{
				Disabled = 0,
				TextOnly,
				WithMnemonics
			};

		private:
			wxAlignment m_Alignment = wxALIGN_INVALID;
			wxEllipsizeMode m_EllipsizeMode = wxELLIPSIZE_END;
			MarkupMode m_MarkupMode = MarkupMode::Disabled;
			ItemAttributes m_Attributes;
			
			Column* m_Column = nullptr;
			wxGCDC* m_GraphicsDC = nullptr;
			wxDC* m_RegularDC = nullptr;

		private:
			void BeginRendering(Column& column, wxGCDC& graphicsDC, wxDC* regularDC = nullptr)
			{
				m_Column = &column;
				m_GraphicsDC = &graphicsDC;
				m_RegularDC = regularDC;
			}
			void EndRendering()
			{
				m_Column = nullptr;
				m_GraphicsDC = nullptr;
				m_RegularDC = nullptr;
			}
			void CallDrawCellBackground(const wxRect& cellRect, CellState cellState);
			void CallDrawCellContent(const wxRect& cellRect, CellState cellState);
			void CallOnActivateCell(Node& node, const wxRect& cellRect, const wxMouseEvent* mouseEvent = nullptr);

			void SetupCellAttributes(const Node& node, Column& column, CellState cellState);

		protected:
			const ItemAttributes& GetAttributes() const
			{
				return m_Attributes;
			}
			RenderEngine GetRenderEngine() const
			{
				return RenderEngine(const_cast<Renderer&>(*this));
			}

			virtual wxAny OnActivateCell(Node& node, const wxRect& cellRect, const wxMouseEvent* mouseEvent = nullptr)
			{
				return {};
			}
			virtual bool SetValue(const wxAny& value) = 0;

			virtual bool HasSolidBackground() const;
			virtual bool HasSpecialBackground() const;
			virtual void DrawCellBackground(const wxRect& cellRect, CellState cellState)
			{
			}
			virtual void DrawCellContent(const wxRect& cellRect, CellState cellState) = 0;
			virtual wxSize GetCellSize() const;

		public:
			bool HasRegularDC() const
			{
				return m_RegularDC != nullptr;
			}
			wxDC& GetRegularDC() const
			{
				return *m_RegularDC;
			}

			bool HasGraphicsDC() const
			{
				return m_GraphicsDC != nullptr;
			}
			wxGCDC& GetGraphicsDC() const
			{
				return *m_GraphicsDC;
			}
			wxGraphicsContext& GetGraphicsContext() const
			{
				return *m_GraphicsDC->GetGraphicsContext();
			}

		public:
			Renderer(int alignment = wxALIGN_INVALID)
				:m_Alignment(static_cast<wxAlignment>(alignment))
			{
			}
			virtual ~Renderer() = default;

		public:
			MainWindow* GetMainWindow() const;
			View* GetView() const;
			Column* GetColumn() const
			{
				return m_Column;
			}
			
			virtual wxAlignment GetEffectiveAlignment() const;
			wxAlignment GetAlignment() const
			{
				return m_Alignment;
			}
			void SetAlignment(wxAlignment alignment)
			{
				m_Alignment = alignment;
			}
			void SetAlignment(int alignment)
			{
				m_Alignment = static_cast<wxAlignment>(alignment);
			}

			wxEllipsizeMode GetEllipsizeMode() const
			{
				return m_EllipsizeMode;
			}
			void SetEllipsizeMode(wxEllipsizeMode mode)
			{
				m_EllipsizeMode = mode;
			}

			bool IsMarkupEnabled() const
			{
				return m_MarkupMode != MarkupMode::Disabled;
			}
			bool IsTextMarkupEnabled() const
			{
				return m_MarkupMode == MarkupMode::TextOnly;
			}
			bool IsMarkupWithMnemonicsEnabled() const
			{
				return m_MarkupMode == MarkupMode::WithMnemonics;
			}
		
			void EnableMarkup(bool enable = true)
			{
				m_MarkupMode = enable ? MarkupMode::TextOnly : MarkupMode::Disabled;
			}
			void EnableMarkupWithMnemonics(bool enable = true)
			{
				m_MarkupMode = enable ? MarkupMode::WithMnemonics : MarkupMode::Disabled;
			}
	};
}
