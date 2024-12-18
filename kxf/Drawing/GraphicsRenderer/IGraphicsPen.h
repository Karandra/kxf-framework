#pragma once
#include "Common.h"
#include "IGraphicsObject.h"

namespace kxf
{
	class IGraphicsBrush;
}

namespace kxf
{
	class KXF_API IGraphicsPen: public RTTI::ExtendInterface<IGraphicsPen, IGraphicsObject>
	{
		kxf_RTTI_DeclareIID(IGraphicsPen, {0x9dc1310, 0xcc3e, 0x4a6a, {0xb5, 0x35, 0x60, 0x59, 0x45, 0x29, 0x22, 0x5e}});

		public:
			virtual ~IGraphicsPen() = default;

		public:
			virtual bool IsTransparent() const = 0;

			virtual Color GetColor() const = 0;
			virtual void SetColor(const Color& color) = 0;

			virtual PenStyle GetStyle() const = 0;
			virtual void SetStyle(PenStyle style) = 0;

			virtual DashStyle GetDashStyle() const = 0;
			virtual void SetDashStyle(DashStyle style) = 0;

			virtual LineJoin GetLineJoin() const = 0;
			virtual void SetLineJoin(LineJoin join) = 0;

			virtual LineCap GetLineCap() const = 0;
			virtual void SetLineCap(LineCap cap) = 0;

			virtual LineAlignment GetLineAlignment() const = 0;
			virtual void SetLineAlignment(LineAlignment alignment) = 0;

			virtual float GetWidth() const = 0;
			virtual void SetWidth(float width) = 0;

			virtual std::shared_ptr<IGraphicsBrush> GetBrush() const = 0;
			virtual void SetBrush(std::shared_ptr<IGraphicsBrush> brush) = 0;

			virtual size_t GetDashPattern(float* dashes, size_t maxCount) const = 0;
			std::vector<float> GetDashPattern() const
			{
				std::vector<float> dashes;
				dashes.resize(GetDashPattern(nullptr, 0));

				GetDashPattern(dashes.data(), dashes.size());
				return dashes;
			}

			virtual void SetDashPattern(const float* dashes, size_t count) = 0;
			void SetDashPattern(const std::vector<float>& dashes)
			{
				SetDashPattern(dashes.data(), dashes.size());
			}

			template<size_t N>
			void SetDashPattern(const float(&dashes)[N])
			{
				SetDashPattern(dashes, N);
			}

			template<size_t N>
			void SetDashPattern(const std::array<float, N>& dashes)
			{
				SetDashPattern(dashes.data(), dashes.size());
			}
	};
}
