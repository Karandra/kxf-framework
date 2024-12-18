#pragma once
#include "Common.h"
#include "FSPath.h"

namespace kxf
{
	class KXF_API LegacyVolume final
	{
		public:
			static Enumerator<LegacyVolume> EnumVolumes();

			static LegacyVolume FromChar(const UniChar& value)
			{
				LegacyVolume drive;
				drive.AssignFromChar(value);
				return drive;
			}
			static LegacyVolume FromChar(char value)
			{
				return FromChar(UniChar(value));
			}
			static LegacyVolume FromChar(wchar_t value)
			{
				return FromChar(UniChar(value));
			}
			static LegacyVolume FromChar(const String& value)
			{
				LegacyVolume drive;
				if (!value.IsEmpty())
				{
					drive.AssignFromChar(value[0]);
				}
				return drive;
			}
			static LegacyVolume FromIndex(int index)
			{
				LegacyVolume drive;
				drive.AssignFromIndex(index);
				return drive;
			}

		private:
			void AssignFromChar(const UniChar& value);
			void AssignFromIndex(int index);
			String DoGetPath() const;

		private:
			char m_Drive = 0;

		public:
			LegacyVolume() = default;
			LegacyVolume(const LegacyVolume&) = default;

		public:
			bool IsValid() const;
			bool DoesExist() const;

			FSPath GetPath() const;
			int GetIndex() const;
			char GetChar() const;

		public:
			explicit operator bool() const noexcept
			{
				return IsValid();
			}
			bool operator!() const noexcept
			{
				return !IsValid();
			}
	};
}
