#include "kxf-pch.h"
#include "DynamicLibraryEvent.h"
#include "DynamicLibrary.h"

namespace kxf
{
	DynamicLibrary DynamicLibraryEvent::GetLibrary() const
	{
		DynamicLibrary library;
		library.AttachHandle(m_Handle);
		return library;
	}
}
