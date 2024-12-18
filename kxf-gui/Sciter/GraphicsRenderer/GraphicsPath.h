#pragma once
#include "Common.h"

namespace kxf::Sciter
{
	struct GraphicsPathHandle;
	class ScriptValue;
}

namespace kxf::Sciter
{
	class KXF_API GraphicsPath final: public HandleWrapper<GraphicsPath, GraphicsPathHandle>
	{
		friend class HandleWrapper<GraphicsPath, GraphicsPathHandle>;
		
		private:
			GraphicsPathHandle* m_Handle = nullptr;

		private:
			bool DoAcquire(GraphicsPathHandle* handle);
			void DoRelease();

		public:
			GraphicsPath() = default;
			GraphicsPath(GraphicsPathHandle* handle)
				:HandleWrapper(handle)
			{
			}
			GraphicsPath(const GraphicsPath& other)
				:HandleWrapper(other)
			{
			}
			GraphicsPath(GraphicsPath&& other)
				:HandleWrapper(std::move(other))
			{
			}
			GraphicsPath(const ScriptValue& value);

		public:
			ScriptValue ToScriptValue() const;
			
			void CloseSubPath();
			void MoveToPoint(const wxPoint2DDouble& pos);

			void AddLineToPoint(const wxPoint2DDouble& pos);
			void AddCurveToPoint(const wxPoint2DDouble& c1, const wxPoint2DDouble& c2, const wxPoint2DDouble& e);
			void AddQuadCurveToPoint(double cx, double cy, double x, double y);
			void AddArcToPoint(const wxPoint2DDouble& c, const wxPoint2DDouble& r, double angle, bool isLargeArc = false, bool closkwise = false);

		public:
			GraphicsPath& operator=(const GraphicsPath& other)
			{
				CopyFrom(other);
				return *this;
			}
			GraphicsPath& operator=(GraphicsPath&& other)
			{
				MoveFrom(other);
				return *this;
			}
			GraphicsPath& operator=(GraphicsPathHandle* handle)
			{
				CopyFrom(handle);
				return *this;
			}
	};
}
