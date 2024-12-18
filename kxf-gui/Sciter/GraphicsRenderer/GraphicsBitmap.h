#pragma once
#include "Common.h"
#include "kxf/Drawing/BitmapImage.h"
#include "kxf/IO/IStream.h"

namespace kxf::Sciter
{
	struct GraphicsBitmapHandle;
	class GraphicsContext;
	class ScriptValue;
}

namespace kxf::Sciter
{
	class KXF_API GraphicsBitmap final: public HandleWrapper<GraphicsBitmap, GraphicsBitmapHandle>
	{
		friend class HandleWrapper<GraphicsBitmap, GraphicsBitmapHandle>;

		public:
			using TDrawOnFunc = std::function<void(GraphicsContext&, const Size& size)>;

		private:
			GraphicsBitmapHandle* m_Handle = nullptr;

		private:
			bool DoAcquire(GraphicsBitmapHandle* handle);
			void DoRelease();

		public:
			GraphicsBitmap() = default;
			GraphicsBitmap(GraphicsBitmapHandle* handle)
				:HandleWrapper(handle)
			{
			}
			GraphicsBitmap(const GraphicsBitmap& other)
				:HandleWrapper(other)
			{
			}
			GraphicsBitmap(GraphicsBitmap&& other)
				:HandleWrapper(std::move(other))
			{
			}
			GraphicsBitmap(const Size& size, bool withAlpha);
			GraphicsBitmap(const Size& size, const char* pixmapData, bool withAlpha)
			{
				CreateFromPixmap(size, pixmapData, withAlpha);
			}
			GraphicsBitmap(const BitmapImage& image);
			GraphicsBitmap(const GDIBitmap& bitmap);
			GraphicsBitmap(IInputStream& stream)
			{
				Load(stream);
			}
			GraphicsBitmap(const ScriptValue& value);

		public:
			// Construct image from B[n+0], G[n+1], R[n+2], A[n+3] data.
			// Size of pixmap data is (pixmapWidth * pixmapHeight * 4).
			bool CreateFromPixmap(const Size& size, const char* pixmapData, bool withAlpha);

			bool Load(IInputStream& stream);
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format, int quality = 100) const;

			bool Clear(const Color& color);
			Size GetSize() const;
			bool UsesAlpha() const;

			BitmapImage ConvertToImage() const;
			GDIBitmap ConvertToBitmap() const;
			ScriptValue ToScriptValue() const;

			void DrawOn(TDrawOnFunc func);

		public:
			GraphicsBitmap& operator=(const GraphicsBitmap& other)
			{
				CopyFrom(other);
				return *this;
			}
			GraphicsBitmap& operator=(GraphicsBitmap&& other)
			{
				MoveFrom(other);
				return *this;
			}
			GraphicsBitmap& operator=(GraphicsBitmapHandle* handle)
			{
				CopyFrom(handle);
				return *this;
			}
	};
}
