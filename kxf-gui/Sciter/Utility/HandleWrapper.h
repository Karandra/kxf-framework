#pragma once

namespace kxf::Sciter
{
	template<class TObject, class THandle>
	class HandleWrapper
	{
		protected:
			THandle* m_Handle = nullptr;

		private:
			TObject& GetSelf() noexcept
			{
				return static_cast<TObject&>(*this);
			}
			const TObject& GetSelf() const noexcept
			{
				return static_cast<const TObject&>(*this);
			}

		protected:
			void Acquire(THandle* handle) noexcept
			{
				Release();
				if (GetSelf().DoAcquire(handle))
				{
					m_Handle = handle;
				}
			}
			void Release() noexcept
			{
				if (m_Handle)
				{
					GetSelf().DoRelease();
				}
				m_Handle = nullptr;
			}

			void CopyFrom(const HandleWrapper& other) noexcept
			{
				Release();
				Acquire(other.m_Handle);
			}
			void CopyFrom(THandle* handle) noexcept
			{
				Release();
				Acquire(handle);
			}
			void MoveFrom(HandleWrapper& other) noexcept
			{
				Release();
				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;
			}
			
		protected:
			HandleWrapper() noexcept = default;
			HandleWrapper(THandle* handle)
			{
				CopyFrom(handle);
			}
			HandleWrapper(const HandleWrapper& other) noexcept
			{
				CopyFrom(other);
			}
			HandleWrapper(HandleWrapper&& other) noexcept
			{
				MoveFrom(other);
			}
			~HandleWrapper() noexcept
			{
				Release();
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Handle == nullptr;
			}
			void MakeNull() noexcept
			{
				Release();
			}
			THandle* GetHandle() const noexcept
			{
				return m_Handle;
			}
			
			HandleWrapper& AttachHandle(THandle* handle) noexcept
			{
				if (IsNull())
				{
					m_Handle = handle;
				}
				return *this;
			}
			THandle* DetachHandle() noexcept
			{
				THandle* handle = m_Handle;
				m_Handle = nullptr;
				return handle;
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			bool operator==(const HandleWrapper& other) const noexcept
			{
				return m_Handle == other.m_Handle;
			}
			bool operator!=(const HandleWrapper& other) const noexcept
			{
				return !(*this == other);
			}

			HandleWrapper& operator=(const HandleWrapper& other) noexcept
			{
				CopyFrom(other);
				return *this;
			}
			HandleWrapper& operator=(HandleWrapper&& other) noexcept
			{
				MoveFrom(other);
				return *this;
			}
	};
}
