#pragma once
#include "../Common.h"
#include "../IGraphicsPen.h"

namespace kxf::Drawing
{
	class KXF_API NullGraphicsPen final: public IGraphicsPen
	{
		public:
			NullGraphicsPen() noexcept = default;

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return true;
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return other.IsNull();
			}
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return nullptr;
			}

			IGraphicsRenderer& GetRenderer() override
			{
				return Drawing::Private::GetNullGraphicsRenderer();
			}
			void* GetNativeHandle() const
			{
				return nullptr;
			}

			// IGraphicsPen
			bool IsTransparent() const override
			{
				return true;
			}

			Color GetColor() const override
			{
				return {};
			}
			void SetColor(const Color& color) override
			{
			}

			PenStyle GetStyle() const override
			{
				return PenStyle::None;
			}
			void SetStyle(PenStyle style) override
			{
			}

			DashStyle GetDashStyle() const override
			{
				return DashStyle::None;
			}
			void SetDashStyle(DashStyle style) override
			{
			}

			LineJoin GetLineJoin() const override
			{
				return LineJoin::None;
			}
			void SetLineJoin(LineJoin join) override
			{
			}

			LineCap GetLineCap() const override
			{
				return LineCap::None;
			}
			void SetLineCap(LineCap cap) override
			{
			}

			LineAlignment GetLineAlignment() const override
			{
				return LineAlignment::Center;
			}
			void SetLineAlignment(LineAlignment alignment) override
			{
			}

			float GetWidth() const override
			{
				return 0.0f;
			}
			void SetWidth(float width) override
			{
			}

			std::shared_ptr<IGraphicsBrush> GetBrush() const override
			{
				return nullptr;
			}
			void SetBrush(std::shared_ptr<IGraphicsBrush> brush) override
			{
			}

			size_t GetDashPattern(float* dashes, size_t maxCount) const override
			{
				return 0;
			}
			void SetDashPattern(const float* dashes, size_t count) override
			{
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsPen NullGraphicsPen;
}
