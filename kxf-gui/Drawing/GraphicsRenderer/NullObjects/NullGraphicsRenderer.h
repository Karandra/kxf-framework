#pragma once
#include "../Common.h"
#include "../IGraphicsRenderer.h"
#include "../IGraphicsContext.h"
#include "../IGraphicsTexture.h"
#include "../IGraphicsPen.h"
#include "../IGraphicsBrush.h"
#include "../IGraphicsPath.h"
#include "../IGraphicsFont.h"

namespace kxf::Drawing
{
	class KXF_API NullGraphicsRenderer final: public IGraphicsRenderer
	{
		public:
			// IGraphicsRenderer
			String GetName() const override
			{
				return {};
			}
			Version GetVersion() const override
			{
				return {};
			}

			std::shared_ptr<IGraphicsContext> CreateContext(std::shared_ptr<IGraphicsTexture> texture, IWidget* widget = nullptr) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsContext> CreateWidgetContext(IWidget& widget) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsContext> CreateWidgetClientContext(IWidget& widget) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsContext> CreateWidgetPaintContext(IWidget& widget) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsContext> CreateMeasuringContext(IWidget* widget = nullptr) override
			{
				return nullptr;
			}

			std::shared_ptr<IGraphicsContext> CreateLegacyContext(std::shared_ptr<IGraphicsTexture> texture, wxWindow* window = nullptr) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsContext> CreateLegacyContext(wxDC& dc, const Size& size = Size::UnspecifiedSize()) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsContext> CreateLegacyWindowContext(wxWindow& window) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsContext> CreateLegacyWindowClientContext(wxWindow& window) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsContext> CreateLegacyWindowPaintContext(wxWindow& window) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsContext> CreateLegacyMeasuringContext(wxWindow* window = nullptr) override
			{
				return nullptr;
			}

		public:
			// Pen and brush functions
			const IGraphicsPen& GetTransparentPen() const override;
			const IGraphicsBrush& GetTransparentBrush() const override;

			std::shared_ptr<IGraphicsPen> CreatePen(const Color& color, float width = 1.0f) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsSolidBrush> CreateSolidBrush(const Color& color) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsTextureBrush> CreateTextureBrush(const BitmapImage& image) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsLinearGradientBrush> CreateLinearGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform = {}) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsRadialGradientBrush> CreateRadialGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform = {}) override
			{
				return nullptr;
			}

			// Path functions
			std::shared_ptr<IGraphicsPath> CreatePath() override
			{
				return nullptr;
			}

			// Texture functions
			std::shared_ptr<IGraphicsTexture> CreateTexture() override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsTexture> CreateTexture(const IImage2D& image) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsTexture> CreateTexture(const BitmapImage& image) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsTexture> CreateTexture(const SizeF& size, const Color& color) override
			{
				return nullptr;
			}

			// Text functions
			std::shared_ptr<IGraphicsFont> CreateFont() override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsFont> CreateFont(const Font& font) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsFont> CreateFont(float pointSize, const String& faceName) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsFont> CreateFont(const SizeF& pixelSize, const String& faceName) override
			{
				return nullptr;
			}
	};
}
namespace kxf
{
	inline const IGraphicsRenderer& NullGraphicsRenderer = Drawing::Private::GetNullGraphicsRenderer();
}
