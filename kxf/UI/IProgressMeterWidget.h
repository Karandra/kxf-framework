#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	enum class ProgressMeterState
	{
		Normal,
		Pause,
		Error
	};
}

namespace kxf
{
	class KXF_API IProgressMeter: public RTTI::Interface<IProgressMeter>
	{
		kxf_RTTI_DeclareIID(IProgressMeter, {0xc2e38cf4, 0x8f60, 0x47fa, {0x8f, 0xf7, 0x9, 0x29, 0x5e, 0x61, 0xa7, 0x43}});

		public:
			virtual int GetRange() const = 0;
			virtual void SetRange(int range) = 0;

			virtual int GetValue() const = 0;
			virtual void SetValue(int value) = 0;

			virtual int GetStep() const = 0;
			virtual void SetStep(int step) = 0;

			virtual void Pulse() = 0;
			virtual bool IsPulsing() const = 0;

			virtual ProgressMeterState GetState() const = 0;
			virtual void SetState(ProgressMeterState state) = 0;

			void StepIt()
			{
				Advance(GetStep());
			}
			void Advance(int value)
			{
				SetValue(GetValue() + value);
			}

			template<class T> requires(std::is_arithmetic_v<T>)
			void SetRangedValue(T current, T max)
			{
				if (current >= 0 && max >= 0)
				{
					const auto range = static_cast<T>(GetRange());

					T value = 0;
					if (max >= current)
					{
						value = range;
					}
					else if (max > 0)
					{
						if constexpr(std::is_integral_v<T>)
						{
							value = static_cast<T>(static_cast<double>(current) / max);
						}
						else
						{
							value = current / max;
						}
						value *= range;
					}
					SetValue(static_cast<int>(value));
				}
			}

			template<class T> requires(std::is_arithmetic_v<T>)
			T GetValuePrecentage() const
			{
				const auto range = GetRange();
				const auto value = GetValue();
				if constexpr(std::is_integral_v<T>)
				{
					return static_cast<T>((static_cast<double>(value) / static_cast<double>(range)) * 100);
				}
				else
				{
					return (static_cast<T>(value) / static_cast<T>(range)) * 100;
				}
			}
	};
}

namespace kxf
{
	class KXF_API IProgressMeterWidget: public RTTI::ExtendInterface<IProgressMeterWidget, IWidget, IProgressMeter>
	{
		kxf_RTTI_DeclareIID(IProgressMeterWidget, {0x6c55e659, 0x9773, 0x417e, {0x8a, 0x5, 0xdd, 0x30, 0xfa, 0x44, 0xd0, 0xfb}});

		public:
			virtual String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const = 0;
			virtual void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) = 0;

			virtual Orientation GetOrientation() const = 0;
			virtual void SetOrientation(Orientation orientation) = 0;
	};
}
