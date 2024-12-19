#pragma once
#include "Common.h"
#include "Color.h"
#include "ColorDepth.h"
#include "Geometry.h"
#include "ImageDefines.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IInputStream;
	class IOutputStream;
	class BitmapImage;
}

namespace kxf
{
	class KXF_API IImage2D: public RTTI::Interface<IImage2D>
	{
		kxf_RTTI_DeclareIID_Using(IImage2D, ImageFormat::Any.ToNativeUUID());

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		public:
			virtual ~IImage2D() = default;

		public:
			virtual bool IsNull() const = 0;
			virtual bool IsSameAs(const IImage2D& other) const = 0;
			virtual std::shared_ptr<IImage2D> CloneImage2D() const = 0;

			// Create, save and load
			virtual void Create(const Size& size) = 0;
			virtual bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos) = 0;
			virtual bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const = 0;

			// Properties
			virtual Size GetSize() const = 0;
			virtual int GetWidth() const
			{
				return GetSize().GetWidth();
			}
			virtual int GetHeight() const
			{
				return GetSize().GetHeight();
			}
			virtual ColorDepth GetColorDepth() const = 0;
			virtual UniversallyUniqueID GetFormat() const = 0;

			// Options
			virtual std::optional<String> GetOption(const String& name) const = 0;
			virtual std::optional<int> GetOptionInt(const String& name) const = 0;
			virtual void SetOption(const String& name, const String& value) = 0;
			virtual void SetOption(const String& name, int value) = 0;

			// Conversion
			virtual BitmapImage ToBitmapImage(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const = 0;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}
