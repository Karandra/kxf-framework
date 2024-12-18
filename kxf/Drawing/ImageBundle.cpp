#include "kxf-pch.h"
#include "ImageBundle.h"
#include "GDIRenderer/GDIIcon.h"
#include "kxf/IO/IStream.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/Application/IGUIApplication.h"
#include "kxf/UI/ITopLevelWidget.h"
#include <wx/iconbndl.h>

namespace
{
	constexpr kxf::Size g_DefaultIconSize = {32, 32};
	constexpr kxf::Size g_DefaultIconSizeSmall = {16, 16};
}

namespace kxf
{
	// ImageBundle
	ImageBundle::ImageBundle(const wxIconBundle& other)
	{
		for (size_t i = 0; i < other.GetIconCount(); i++)
		{
			if (!m_Items.emplace_back(GDIIcon(other.GetIconByIndex(i)).ToBitmapImage()))
			{
				m_Items.pop_back();
			}
		}
	}
	ImageBundle::ImageBundle(const BitmapImage& other)
	{
		if (other)
		{
			m_Items.emplace_back(other);
		}
	}

	bool ImageBundle::IsNull() const
	{
		return m_Items.empty();
	}
	bool ImageBundle::IsSameAs(const ImageBundle& other) const
	{
		if (this == &other)
		{
			return true;
		}
		else if (m_Items.size() == other.m_Items.size())
		{
			for (size_t i = 0; i < m_Items.size(); i++)
			{
				if (!m_Items[i].IsSameAs(other.m_Items[i]))
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool ImageBundle::Load(IInputStream& stream, const UniversallyUniqueID& format, size_t index)
	{
		const auto initialPos = stream.TellI();
		const size_t imageCount = BitmapImage::GetImageCount(stream, format);

		size_t loadedCount = 0;
		UniversallyUniqueID actualFormat = format;
		for (size_t i = 0; i < imageCount; i++)
		{
			// The call to 'Load' for the first sub-image updated the stream position
			// but we need to start reading the subsequent sub-image at the image beginning too.
			if (i != 0)
			{
				stream.SeekI(initialPos, IOStreamSeek::FromStart);
			}

			BitmapImage& image = m_Items.emplace_back();
			if (image.Load(stream, actualFormat, static_cast<int>(i)))
			{
				loadedCount++;
				if (actualFormat == ImageFormat::Any || actualFormat == ImageFormat::None)
				{
					// Store the type so that we don't need to try all handlers again
					// for the subsequent images, they should all be of the same type.
					actualFormat = image.GetFormat();
				}
			}
			else
			{
				m_Items.pop_back();
			}
		}
		return loadedCount != 0;
	}
	bool ImageBundle::Save(IOutputStream& stream, const UniversallyUniqueID& format) const
	{
		// TODO: Implement ImageBundle saving
		return false;
	}

	void ImageBundle::AddImage(const BitmapImage& image)
	{
		if (image)
		{
			m_Items.emplace_back(image);
		}
	}
	BitmapImage ImageBundle::GetImage(Size desiredSize, FlagSet<ImageBundleFlag> sizeFallback) const
	{
		if (desiredSize.IsFullySpecified() || desiredSize == Size::UnspecifiedSize())
		{
			// We need the standard system icon size when using system icon fallback
			Size systemIcon;
			Size systemSmallIcon;
			if (sizeFallback.Contains(ImageBundleFlag::SystemSize) || sizeFallback.Contains(ImageBundleFlag::SystemSizeSmall))
			{
				std::shared_ptr<IWidget> topWidget;
				if (auto app = IGUIApplication::GetInstance())
				{
					topWidget = app->GetTopWidget();
				}

				// Get the system icon size
				if (sizeFallback.Contains(ImageBundleFlag::SystemSize))
				{
					systemIcon = System::GetMetric(SystemSizeMetric::Icon, topWidget ? topWidget->GetWxWindow() : nullptr);
					sizeFallback.Add(ImageBundleFlag::NearestLarger, !systemSmallIcon.IsFullySpecified());
					systemIcon.SetDefaults(g_DefaultIconSize);
				}
				if (sizeFallback.Contains(ImageBundleFlag::SystemSizeSmall))
				{
					systemSmallIcon = System::GetMetric(SystemSizeMetric::IconSmall, topWidget ? topWidget->GetWxWindow() : nullptr);
					sizeFallback.Add(ImageBundleFlag::NearestLarger, !systemSmallIcon.IsFullySpecified());
					systemIcon.SetDefaults(g_DefaultIconSizeSmall);
				}
			}

			// If 'desiredSize' is unspecified, we use system default icon size by convention
			if (!desiredSize.IsFullySpecified())
			{
				if (systemIcon.IsFullySpecified())
				{
					desiredSize = systemIcon;
				}
				else if (systemSmallIcon.IsFullySpecified())
				{
					desiredSize = systemSmallIcon;
				}
			}

			// Iterate over all icons searching for the exact match or the closest icon for 'SizeFallback::NearestLarger'
			const BitmapImage* imageBest = nullptr;
			int bestDiff = 0;
			bool bestIsLarger = false;
			bool bestIsSystem = false;

			for (const BitmapImage& image: m_Items)
			{
				const Size size = image.GetSize();

				// Exact match ends search immediately
				if (size == desiredSize)
				{
					return image;
				}

				if ((sizeFallback.Contains(ImageBundleFlag::SystemSize) && size == systemIcon) || (sizeFallback.Contains(ImageBundleFlag::SystemSizeSmall) && size == systemSmallIcon))
				{
					imageBest = &image;
					bestIsSystem = true;
					continue;
				}

				if (!bestIsSystem && sizeFallback.Contains(ImageBundleFlag::NearestLarger))
				{
					const bool iconLarger = (size.GetWidth() >= desiredSize.GetWidth()) && (size.GetHeight() >= desiredSize.GetHeight());
					const int iconDiff = std::abs(size.GetWidth() - desiredSize.GetWidth()) + std::abs(size.GetHeight() - desiredSize.GetHeight());

					// Use current icon as candidate for the best icon, if either:
					// - We have no candidate yet.
					// - We have no candidate larger than desired size and current icon is.
					// - Current icon is closer to desired size than candidate.
					if (!imageBest || (!bestIsLarger && iconLarger) || (iconLarger && (iconDiff < bestDiff)))
					{
						imageBest = &image;
						bestIsLarger = iconLarger;
						bestDiff = iconDiff;

						continue;
					}
				}
			}

			if (imageBest)
			{
				return *imageBest;
			}
		}
		return {};
	}

	wxIconBundle ImageBundle::ToWxIconBundle() const
	{
		wxIconBundle iconBundle;
		for (const BitmapImage& image: m_Items)
		{
			iconBundle.AddIcon(image.ToGDIIcon().ToWxIcon());
		}
		return iconBundle;
	}
}
