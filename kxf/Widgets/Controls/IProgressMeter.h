#pragma once
#include "kxf/Widgets/Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf::UI
{
	class KXF_API IProgressMeter: public RTTI::Interface<IProgressMeter>
	{
		kxf_RTTI_DeclareIID(IProgressMeter, {0x56b2420b, 0x464, 0x4075, {0x87, 0x5a, 0x53, 0x3c, 0x30, 0xd0, 0x3e, 0x97}});

		protected:
			virtual int DoGetRange() const = 0;
			virtual void DoSetRange(int range) = 0;

			virtual int DoGetValue() const = 0;
			virtual void DoSetValue(int value) = 0;

			virtual int DoGetStep() const = 0;
			virtual void DoSetStep(int step) = 0;

			virtual void DoPulse() = 0;
			virtual bool DoIsPulsing() const = 0;

		public:
			virtual ~IProgressMeter() = default;

		public:
			int GetRange() const
			{
				return DoGetRange();
			}
			void SetRange(int range)
			{
				if (range >= 0)
				{
					DoSetRange(range);
				}
			}

			int GetValue() const
			{
				return DoGetValue();
			}
			void SetValue(int value)
			{
				if (value >= 0)
				{
					DoSetValue(value);
				}
			}
			void SetValue(int64_t current, int64_t max)
			{
				if (current >= 0 && max >= 0)
				{
					const int range = DoGetRange();

					int value = 0;
					if (max >= current)
					{
						value = range;
					}
					else if (max > 0)
					{
						value = (static_cast<double>(current) / max) * range;
					}
					DoSetValue(value);
				}
			}

			int GetStep() const
			{
				return DoGetStep();
			}
			void SetStep(int step)
			{
				const int range = DoGetRange();
				DoSetStep(std::clamp(step, -range, range));
			}

			void Advance(int value)
			{
				DoSetValue(std::clamp(DoGetValue() + value, 0, DoGetRange()));
			}
			void StepIt()
			{
				Advance(DoGetStep());
			}

			void Pulse()
			{
				DoPulse();
			}
			bool IsPulsing() const
			{
				return DoIsPulsing();
			}
	};
}
