#pragma once
#include "Common.h"
#include "kxf/System/COM.h"
#include "kxf/System/DynamicLibrary.h"

namespace kxf::SevenZip
{
	class KXF_API Library final
	{
		public:
			static Library& GetInstance()
			{
				static Library instance;
				return instance;
			}
		
		private:
			DynamicLibrary m_Library;
			void* m_CreateObjectFunc = nullptr;

		private:
			Library() = default;
			~Library()
			{
				Unload();
			}

		public:
			bool IsLoaded() const noexcept
			{
				return m_Library && m_CreateObjectFunc;
			}
			DynamicLibrary& GetLibrary() noexcept
			{
				return m_Library;
			}

			bool Load();
			bool Load(const FSPath& libraryPath);
			void Unload() noexcept;

			bool CreateObject(const NativeUUID& classID, const NativeUUID& interfaceID, void** object) const;

			template<std::derived_from<IUnknown> T>
			COMPtr<T> CreateObject(const NativeUUID& classID, const NativeUUID& interfaceID) const
			{
				COMPtr<T> object;
				if (CreateObject(classID, interfaceID, object.GetAddress()))
				{
					return object;
				}
				return nullptr;
			}
	};
}
