#pragma once
#include "Common.h"
#include "WxGraphicsRenderer.h"
#include "../Font.h"
#include "../GDIRenderer/GDIFont.h"
#include "../GraphicsRenderer/IGraphicsFont.h"
#include <wx/graphics.h>

namespace kxf
{
	class KXF_API WxGraphicsFont: public RTTI::ExtendInterface<WxGraphicsFont, IGraphicsFont>
	{
		kxf_RTTI_DeclareIID(WxGraphicsFont, {0x8f405c52, 0x6a52, 0x49e5, {0x9a, 0xe8, 0x36, 0x1c, 0xc9, 0x8a, 0xb3, 0x24}});

		protected:
			WxGraphicsRenderer* m_Renderer = nullptr;
			wxGraphicsFont m_Graphics;
			GDIFont m_Font;

			bool m_Initialized = false;

		private:
			void Initialize()
			{
				if (!m_Initialized)
				{
					if (m_Font)
					{
						m_Graphics = m_Renderer->Get().CreateFont(m_Font.ToWxFont());
					}
					else
					{
						m_Graphics = {};
					}
					m_Initialized = true;
				}
			}
			void Invalidate()
			{
				m_Initialized = false;
			}

		public:
			WxGraphicsFont() noexcept = default;
			WxGraphicsFont(WxGraphicsRenderer& rendrer)
				:m_Renderer(&rendrer)
			{
			}
			WxGraphicsFont(WxGraphicsRenderer& rendrer, const Font& font)
				:m_Renderer(&rendrer), m_Font(font)
			{
			}
			WxGraphicsFont(WxGraphicsRenderer& rendrer, const GDIFont& font)
				:m_Renderer(&rendrer), m_Font(font)
			{
			}
			WxGraphicsFont(WxGraphicsRenderer& rendrer, float pointSize, const String& faceName)
				:m_Renderer(&rendrer), m_Font(static_cast<double>(pointSize), FontFamily::Default, FontStyle::Normal, FontWeight::Normal, faceName)
			{
			}
			WxGraphicsFont(WxGraphicsRenderer& rendrer, const SizeF& pixelSize, const String& faceName)
				:m_Renderer(&rendrer), m_Font(pixelSize, FontFamily::Default, FontStyle::Normal, FontWeight::Normal, faceName)
			{
			}

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_Font.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<WxGraphicsFont>())
				{
					return m_Font.IsSameAs(object->m_Font);
				}
				return false;
			}
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_shared<WxGraphicsFont>(*this);
			}

			WxGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Graphics.GetGraphicsData();
			}

			// IGraphicsFont
			String GetFaceName() const override
			{
				return m_Font.GetFaceName();
			}
			void SetFaceName(const String& faceName) override
			{
				m_Font.SetFaceName(faceName);
				Invalidate();
			}

			FontFamily GetFamily() const override
			{
				return m_Font.GetFamily();
			}
			void SetFamily(FontFamily family) override
			{
				m_Font.SetFamily(family);
				Invalidate();
			}

			FontMetricsF GetMetrics() const override;

			float GetPointSize() const override
			{
				return static_cast<float>(m_Font.GetPointSize());
			}
			void SetPointSize(float pointSize) override
			{
				m_Font.SetPointSize(pointSize);
				Invalidate();
			}

			SizeF GetPixelSize() const override
			{
				return m_Font.GetPixelSize();
			}
			void SetPixelSize(const SizeF& pixelSize) override
			{
				m_Font.SetPixelSize(pixelSize);
				Invalidate();
			}

			Font ToFont() const override
			{
				return m_Font;
			}
			bool FromFont(const Font& font) override
			{
				m_Font = font;
				Invalidate();

				return !m_Font.IsNull();
			}

			// WxGraphicsFont
			const wxGraphicsFont& Get() const
			{
				const_cast<WxGraphicsFont&>(*this).Initialize();
				return m_Graphics;
			}
			wxGraphicsFont& Get()
			{
				Initialize();
				return m_Graphics;
			}

			const GDIFont& GetFont() const
			{
				return m_Font;
			}
			GDIFont& GetFont()
			{
				return m_Font;
			}

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}
	};
}
