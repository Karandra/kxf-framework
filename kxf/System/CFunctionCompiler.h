#pragma once
#include "Common.h"
#include "Private/CFunctionCompiler.h"
#include <array>

namespace kxf::FFI
{
	class KXF_API CFunctionCompiler
	{
		private:
			Private::CInterface m_CInterface;
			std::array<const Private::CType*, 16> m_ArgumentTypes;
			Private::CClosure* m_Closure = nullptr;
			void* m_Code = nullptr;
			Private::CStatus m_Status = Private::CStatus::Unknown;

		private:
			bool IsCreated() const noexcept;
			bool SetParameterAt(size_t index, TypeID type) noexcept;

		protected:
			virtual void Execute(void** arguments, void* returnValue) noexcept = 0;

		public:
			CFunctionCompiler() noexcept
			{
				m_ArgumentTypes.fill(nullptr);
			}
			CFunctionCompiler(CFunctionCompiler&& other) noexcept
			{
				*this = std::move(other);
			}
			CFunctionCompiler(const CFunctionCompiler&) = delete;
			virtual ~CFunctionCompiler()
			{
				Destroy();
			}

		public:
			bool IsNull() const noexcept
			{
				return !IsCreated();
			}
			bool Create() noexcept;
			void Destroy() noexcept;

			const void* GetCode() const noexcept
			{
				return m_Code;
			}
			size_t GetCodeSize() const noexcept
			{
				return m_CInterface.m_CodeSize;
			}
			
			template<class T> requires(std::is_function_v<std::remove_pointer_t<T>>)
			T* GetFunctionPointer() const noexcept
			{
				return reinterpret_cast<T*>(m_Code);
			}

			size_t GetParameterCount() const noexcept
			{
				return m_CInterface.m_ArgumentCount;
			}
			void ClearParameters() noexcept;
			bool SetParameters(std::initializer_list<TypeID> types) noexcept;
			bool PushParameter(TypeID type) noexcept;
			TypeID PopParameter() noexcept;
			TypeID GetParameterAt(size_t index) const noexcept;

			TypeID GetReturnType() const noexcept;
			void SetReturnType(TypeID type) noexcept;

			ABI GetABI() const noexcept
			{
				return m_CInterface.m_ABI;
			}
			void SetABI(ABI abi) noexcept
			{
				m_CInterface.m_ABI = abi;
			}

		public:
			CFunctionCompiler& operator=(CFunctionCompiler&& other) noexcept;
			CFunctionCompiler& operator=(const CFunctionCompiler&) = delete;
	};
}
