#include "kxf-pch.h"
#include "CFunctionCompiler.h"
#include "kxf/Utility/ScopeGuard.h"
#include <ffi.h>

namespace kxf::FFI::Private
{
	constexpr const ffi_type* GetFFIType(TypeID type) noexcept
	{
		switch (type)
		{
			case TypeID::Void:
			{
				return &ffi_type_void;
			}

			case TypeID::Int8:
			{
				return &ffi_type_sint8;
			}
			case TypeID::Int16:
			{
				return &ffi_type_sint16;
			}
			case TypeID::Int32:
			{
				return &ffi_type_sint32;
			}
			case TypeID::Int64:
			{
				return &ffi_type_sint64;
			}

			case TypeID::UInt8:
			{
				return &ffi_type_uint8;
			}
			case TypeID::UInt16:
			{
				return &ffi_type_uint16;
			}
			case TypeID::UInt32:
			{
				return &ffi_type_uint32;
			}
			case TypeID::UInt64:
			{
				return &ffi_type_uint64;
			}

			case TypeID::Pointer:
			{
				return &ffi_type_pointer;
			}

			case TypeID::Float32:
			{
				return &ffi_type_float;
			}
			case TypeID::Float64:
			{
				return &ffi_type_double;
			}
		};
		return nullptr;
	}
	constexpr size_t GetFFITypeSize(TypeID type) noexcept
	{
		return GetFFIType(type)->size;
	}

	template<class T1, class T2>
	constexpr bool TestType() noexcept
	{
		return sizeof(T1) == sizeof(T2) && alignof(T1) == alignof(T2);
	}

	using TClosureCall = void(*)(CInterface*, void*, void**, CFunctionCompiler*);
	struct CClosure final
	{
		uint8_t m_Trampoline[FFI_TRAMPOLINE_SIZE] = {};
		CInterface* m_CInterface = nullptr;
		TClosureCall m_Callback = nullptr;
		CFunctionCompiler* m_Context = nullptr;
	};
}

namespace kxf::FFI
{
	bool CFunctionCompiler::IsCreated() const noexcept
	{
		using namespace Private;

		// Layout
		static_assert(TestType<CInterface, ffi_cif>(), "[CFunctionCompiler] CInterface invalid layout");
		static_assert(TestType<CType, ffi_type>(), "[CFunctionCompiler] CType invalid layout");
		static_assert(TestType<CStatus, ffi_status>(), "[CFunctionCompiler] CStatus invalid layout");
		static_assert(TestType<CClosure, CClosure>(), "[CFunctionCompiler] CClosure invalid layout");

		// Types
		static_assert(ToInt(TypeID::Void) == FFI_TYPE_VOID);
		static_assert(ToInt(TypeID::Float32) == FFI_TYPE_FLOAT);
		static_assert(ToInt(TypeID::Float64) == FFI_TYPE_DOUBLE);
		static_assert(ToInt(TypeID::Int8) == FFI_TYPE_SINT8);
		static_assert(ToInt(TypeID::UInt8) == FFI_TYPE_UINT8);
		static_assert(ToInt(TypeID::Int16) == FFI_TYPE_SINT16);
		static_assert(ToInt(TypeID::UInt16) == FFI_TYPE_UINT16);
		static_assert(ToInt(TypeID::Int32) == FFI_TYPE_SINT32);
		static_assert(ToInt(TypeID::UInt32) == FFI_TYPE_UINT32);
		static_assert(ToInt(TypeID::Int64) == FFI_TYPE_SINT64);
		static_assert(ToInt(TypeID::UInt64) == FFI_TYPE_UINT64);
		static_assert(ToInt(TypeID::Pointer) == FFI_TYPE_POINTER);

		// ABI
		static_assert(ToInt(ABI::Default) == FFI_DEFAULT_ABI);
		#if _WIN64
		static_assert(ToInt(ABI::Win64) == FFI_WIN64);
		static_assert(ToInt(ABI::GNU64) == FFI_GNUW64);
		#else
		static_assert(ToInt(ABI::SysV) == FFI_SYSV);
		static_assert(ToInt(ABI::StdCall) == FFI_STDCALL);
		static_assert(ToInt(ABI::ThisCall) == FFI_THISCALL);
		static_assert(ToInt(ABI::FastCall) == FFI_FASTCALL);
		static_assert(ToInt(ABI::CDecl) == FFI_MS_CDECL);
		static_assert(ToInt(ABI::Pascal) == FFI_PASCAL);
		static_assert(ToInt(ABI::Register) == FFI_REGISTER);
		#endif

		return m_Closure && m_Code && m_CInterface.m_ReturnType && m_Status == CStatus::Success;
	}
	bool CFunctionCompiler::SetParameterAt(size_t index, TypeID type) noexcept
	{
		using namespace Private;

		if (type != TypeID::Void && index < m_ArgumentTypes.size())
		{
			if (type == TypeID::None)
			{
				m_ArgumentTypes[index] = nullptr;
				return true;
			}
			else if (auto ffiType = GetFFIType(type))
			{
				m_ArgumentTypes[index] = reinterpret_cast<const CType*>(ffiType);
				return true;
			}
		}
		return false;
	}

	bool CFunctionCompiler::Create() noexcept
	{
		if (IsCreated())
		{
			return true;
		}

		Utility::ScopeGuard guard = [&]()
		{
			if (m_Closure)
			{
				ffi_closure_free(m_Closure);
				m_Closure = nullptr;
			}
		};

		// Check for the return type, as nullptr here causes access violation exception later
		if (!m_CInterface.m_ReturnType)
		{
			return false;
		}

		// Check for holes in the arguments type list, causes access violation exception as well
		// Shouldn't be necessary with the push/pop functions now, but just in case
		for (size_t i = 0; i < m_CInterface.m_ArgumentCount; i++)
		{
			if (!m_ArgumentTypes[i])
			{
				return false;
			}
		}

		using namespace Private;
		if (m_Closure = reinterpret_cast<CClosure*>(ffi_closure_alloc(sizeof(ffi_closure), &m_Code)))
		{
			auto cif = reinterpret_cast<ffi_cif*>(&m_CInterface);
			auto closure = reinterpret_cast<ffi_closure*>(m_Closure);
			auto argumentTypes = const_cast<ffi_type**>(reinterpret_cast<const ffi_type**>(m_ArgumentTypes.data()));
			auto returnType = const_cast<ffi_type*>(reinterpret_cast<const ffi_type*>(m_CInterface.m_ReturnType));
			auto abi = static_cast<ffi_abi>(m_CInterface.m_ABI);

			m_Status = static_cast<CStatus>(ffi_prep_cif(cif, abi, m_CInterface.m_ArgumentCount, returnType, argumentTypes));
			if (m_Status == CStatus::Success)
			{
				m_Status = static_cast<CStatus>(ffi_prep_closure_loc(closure, cif, [](ffi_cif* cif, void* returnValue, void** arguments, void* context)
				{
					reinterpret_cast<CFunctionCompiler*>(context)->Execute(arguments, returnValue);
				}, this, m_Code));

				if (m_Status == CStatus::Success)
				{
					guard.Dismiss();
					return true;
				}
			}
		}
		return false;
	}
	void CFunctionCompiler::Destroy() noexcept
	{
		using namespace Private;

		if (m_Closure)
		{
			ffi_closure_free(m_Closure);
			m_Closure = nullptr;
		}

		m_CInterface = {};
		m_ArgumentTypes.fill(nullptr);
		m_Closure = nullptr;
		m_Code = nullptr;
		m_Status = CStatus::Unknown;
	}

	void CFunctionCompiler::ClearParameters() noexcept
	{
		m_ArgumentTypes.fill(nullptr);
		m_CInterface.m_ArgumentCount = 0;
	}
	bool CFunctionCompiler::SetParameters(std::initializer_list<TypeID> types) noexcept
	{
		for (TypeID type: types)
		{
			if (type == TypeID::None || type == TypeID::Void)
			{
				return false;
			}
		}

		ClearParameters();
		for (TypeID type: types)
		{
			PushParameter(type);
		}
		return true;
	}
	bool CFunctionCompiler::PushParameter(TypeID type) noexcept
	{
		if (m_CInterface.m_ArgumentCount >= m_ArgumentTypes.size())
		{
			return false;
		}

		if (SetParameterAt(m_CInterface.m_ArgumentCount, type))
		{
			m_CInterface.m_ArgumentCount++;
			return true;
		}
		return false;
	}
	TypeID CFunctionCompiler::PopParameter() noexcept
	{
		if (m_CInterface.m_ArgumentCount != 0)
		{
			auto& lastArg = m_ArgumentTypes[m_CInterface.m_ArgumentCount - 1];
			TypeID type = lastArg->m_Type;
			lastArg = nullptr;

			m_CInterface.m_ArgumentCount--;
			return type;
		}
		return TypeID::None;
	}
	TypeID CFunctionCompiler::GetParameterAt(size_t index) const noexcept
	{
		if (index < m_CInterface.m_ArgumentCount)
		{
			return m_ArgumentTypes[index]->m_Type;
		}
		return TypeID::None;
	}

	TypeID CFunctionCompiler::GetReturnType() const noexcept
	{
		return m_CInterface.m_ReturnType ? m_CInterface.m_ReturnType->m_Type : TypeID::None;
	}
	void CFunctionCompiler::SetReturnType(TypeID type) noexcept
	{
		using namespace Private;

		m_CInterface.m_ReturnType = reinterpret_cast<const CType*>(GetFFIType(type));
	}

	CFunctionCompiler& CFunctionCompiler::operator=(CFunctionCompiler&& other) noexcept
	{
		// Move CInterface and update its parameters array
		m_CInterface = std::move(other.m_CInterface);
		m_ArgumentTypes = std::move(other.m_ArgumentTypes);
		m_CInterface.m_ArgumentTypes = m_ArgumentTypes.data();

		// Move Closure and update its CInterface and context pointers
		m_Closure = other.m_Closure;
		m_Closure->m_CInterface = &m_CInterface;
		m_Closure->m_Context = this;
		other.m_Closure = nullptr;

		m_Code = std::move(other.m_Code);
		m_Status = std::move(other.m_Status);

		// Reset the other object to its default state
		other.Destroy();

		return *this;
	}
}
