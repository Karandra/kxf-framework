#include "kxf-pch.h"
#include "GraphicsBitmap.h"
#include "GraphicsContext.h"
#include "../ScriptValue.h"
#include "../SciterAPI.h"
#include "../Private/Conversion.h"
#include "kxf/IO/MemoryStream.h"

namespace kxf::Sciter
{
	std::optional<SCITER_IMAGE_ENCODING> MapImageEncoding(const kxf::UniversallyUniqueID& encoding) noexcept
	{
		using namespace kxf;

		if (encoding == ImageFormat::PNG)
		{
			return SCITER_IMAGE_ENCODING::SCITER_IMAGE_ENCODING_PNG;
		}
		else if (encoding == ImageFormat::JPEG)
		{
			return SCITER_IMAGE_ENCODING::SCITER_IMAGE_ENCODING_JPG;
		}
		else if (encoding == ImageFormat::WEBP)
		{
			return SCITER_IMAGE_ENCODING::SCITER_IMAGE_ENCODING_WEBP;
		}
		else if (encoding == ImageFormat::RAW)
		{
			return SCITER_IMAGE_ENCODING::SCITER_IMAGE_ENCODING_RAW;
		}
		return {};
	}
	bool DoGetImageInfo(HIMG image, Size& size, bool& usesAlpha) noexcept
	{
		UINT width = 0;
		UINT height = 0;
		BOOL nativeUsesAlpha = FALSE;
		if (GetGrapchicsAPI()->imageGetInfo(image, &width, &height, &nativeUsesAlpha) == GRAPHIN_OK)
		{
			size.SetWidth(width);
			size.SetHeight(height);
			usesAlpha = nativeUsesAlpha;
			return true;
		}
		return false;
	}
}

namespace kxf::Sciter
{
	bool GraphicsBitmap::DoAcquire(GraphicsBitmapHandle* handle)
	{
		return GetGrapchicsAPI()->imageAddRef(ToSciterImage(handle)) == GRAPHIN_OK;
	}
	void GraphicsBitmap::DoRelease()
	{
		GetGrapchicsAPI()->imageRelease(ToSciterImage(m_Handle));
	}

	GraphicsBitmap::GraphicsBitmap(const Size& size, bool withAlpha)
	{
		HIMG image = nullptr;
		if (GetGrapchicsAPI()->imageCreate(&image, size.GetWidth(), size.GetHeight(), withAlpha) == GRAPHIN_OK)
		{
			Acquire(FromSciterImage(image));
		}
	}
	GraphicsBitmap::GraphicsBitmap(const BitmapImage& image)
	{
		MemoryOutputStream outputStream;
		if (image.Save(outputStream, ImageFormat::PNG))
		{
			MemoryInputStream inputStream(outputStream);
			Load(inputStream);
		}
	}
	GraphicsBitmap::GraphicsBitmap(const GDIBitmap& bitmap)
		:GraphicsBitmap(bitmap.ToBitmapImage())
	{
	}
	GraphicsBitmap::GraphicsBitmap(const ScriptValue& value)
	{
		HIMG image = nullptr;
		if (GetGrapchicsAPI()->vUnWrapImage(ToSciterScriptValue(value.GetNativeValue()), &image))
		{
			Acquire(FromSciterImage(image));
		}
	}

	bool GraphicsBitmap::CreateFromPixmap(const Size& size, const char* pixmapData, bool withAlpha)
	{
		HIMG image = nullptr;
		if (GetGrapchicsAPI()->imageCreateFromPixmap(&image, size.GetWidth(), size.GetHeight(), withAlpha, reinterpret_cast<const BYTE*>(pixmapData)) == GRAPHIN_OK)
		{
			Acquire(FromSciterImage(image));
			return true;
		}
		return false;
	}
	bool GraphicsBitmap::Load(IInputStream& stream)
	{
		std::vector<BYTE> buffer;
		buffer.resize(stream.GetSize().ToBytes());
		if (stream.ReadAll(buffer.data(), buffer.size()))
		{
			HIMG image = nullptr;
			if (GetGrapchicsAPI()->imageLoad(buffer.data(), buffer.size(), &image) == GRAPHIN_OK)
			{
				Acquire(FromSciterImage(image));
				return true;
			}
		}
		return false;
	}
	bool GraphicsBitmap::Save(IOutputStream& stream, const UniversallyUniqueID& format, int quality) const
	{
		auto encoding = MapImageEncoding(format);
		if (!IsNull() && encoding)
		{
			return GetGrapchicsAPI()->imageSave(ToSciterImage(m_Handle), [](void* context, const BYTE* data, UINT size) -> BOOL
			{
				IOutputStream& stream = *reinterpret_cast<IOutputStream*>(context);
				return stream.WriteAll(data, size);
			}, &stream, *encoding, quality) == GRAPHIN_OK;
		}
		return false;
	}

	bool GraphicsBitmap::Clear(const Color& color)
	{
		return GetGrapchicsAPI()->imageClear(ToSciterImage(m_Handle), CreateSciterColor(color)) == GRAPHIN_OK;
	}
	Size GraphicsBitmap::GetSize() const
	{
		Size size = Size::UnspecifiedSize();
		bool usesAlpha = false;
		DoGetImageInfo(ToSciterImage(m_Handle), size, usesAlpha);

		return size;
	}
	bool GraphicsBitmap::UsesAlpha() const
	{
		Size size = Size::UnspecifiedSize();
		bool usesAlpha = false;
		DoGetImageInfo(ToSciterImage(m_Handle), size, usesAlpha);

		return usesAlpha;
	}

	BitmapImage GraphicsBitmap::ConvertToImage() const
	{
		MemoryOutputStream outputStream;
		if (Save(outputStream, ImageFormat::PNG))
		{
			MemoryInputStream inputStream(outputStream);

			BitmapImage image;
			image.Load(inputStream, ImageFormat::PNG);
			return image;
		}
		return {};
	}
	GDIBitmap GraphicsBitmap::ConvertToBitmap() const
	{
		return ConvertToImage().ToGDIBitmap();
	}
	ScriptValue GraphicsBitmap::ToScriptValue() const
	{
		ScriptValue value;
		if (GetGrapchicsAPI()->vWrapImage(ToSciterImage(m_Handle), ToSciterScriptValue(value.GetNativeValue())) == GRAPHIN_OK)
		{
			return value;
		}
		return {};
	}

	void GraphicsBitmap::DrawOn(TDrawOnFunc func)
	{
		class CallContext
		{
			private:
				TDrawOnFunc& m_Func;

			public:
				CallContext(TDrawOnFunc& func)
					:m_Func(func)
				{
				}

			public:
				void Execute(GraphicsContextHandle* handle, const Size& size)
				{
					GraphicsContext graphicsContext(handle);
					std::invoke(m_Func, graphicsContext, size);
				}
		};

		CallContext context(func);
		GetGrapchicsAPI()->imagePaint(ToSciterImage(m_Handle), [](void* context, HGFX handle, UINT width, UINT height)
		{
			reinterpret_cast<CallContext*>(context)->Execute(FromSciterGraphicsContext(handle), Size(width, height));
		}, &context);
	}
}
