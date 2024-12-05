#pragma once
#include "Common.h"

namespace kxf
{
	template<class TValue_>
	class optional_ptr final
	{
		public:
			using TValue = TValue_;

		private:
			TValue* m_Value = nullptr;
			bool m_IsOwned = false;

		private:
			void Destroy() noexcept
			{
				if (m_IsOwned)
				{
					delete m_Value;
				}

				m_IsOwned = false;
				m_Value = nullptr;
			}

		public:
			optional_ptr() noexcept = default;
			optional_ptr(TValue& value) noexcept
			{
				reset(value);
			}
			optional_ptr(std::nullptr_t) noexcept
			{
			}
			optional_ptr(std::unique_ptr<TValue> value) noexcept
			{
				reset(std::move(value));
			}
			
			template<class T>
			optional_ptr(optional_ptr<T>&& other) noexcept
			{
				m_IsOwned = other.is_owned();
				m_Value = other.release();
			}
			
			template<class T>
			optional_ptr(const optional_ptr<T>&) = delete;
			
			~optional_ptr() noexcept
			{
				Destroy();
			}

		public:
			// Observers
			bool is_null() const noexcept
			{
				return m_Value == nullptr;
			}
			bool is_owned() const noexcept
			{
				return m_IsOwned;
			}
			
			template<class T>
			bool is_same_as(const optional_ptr<T>& other) const noexcept
			{
				return m_Value == other.m_Value;
			}

			template<class T>
			bool is_same_as(const std::unique_ptr<T>& other) const noexcept
			{
				return m_Value == other.get();
			}

			bool is_same_as(const TValue& other) const noexcept
			{
				return m_Value == &other;
			}

			TValue* get() const noexcept
			{
				return m_Value;
			}
			std::unique_ptr<TValue> get_unique() noexcept
			{
				return m_IsOwned ? std::unique_ptr<TValue>(release()) : nullptr;
			}
			optional_ptr<TValue> shallow_clone() const noexcept
			{
				if (m_Value)
				{
					return *m_Value;
				}
				return {};
			}

			TValue* operator->() const noexcept
			{
				return m_Value;
			}
			const TValue& operator*() const noexcept
			{
				return *m_Value;
			}
			TValue& operator*() noexcept
			{
				return *m_Value;
			}
			
			// Modifiers
			TValue** operator&() noexcept
			{
				return &m_Value;
			}
			TValue* release() noexcept
			{
				m_IsOwned = false;
				return std::exchange(m_Value, nullptr);
			}
			void reset(std::nullptr_t = nullptr) noexcept
			{
				Destroy();
			}

			template<class T>
			void reset(T& value) noexcept
			{
				Destroy();
				m_Value = &value;
				m_IsOwned = false;
			}

			template<class T>
			void reset(std::unique_ptr<T> value) noexcept
			{
				Destroy();
				m_Value = value.release();
				m_IsOwned = true;
			}

		public:
			template<class T>
			optional_ptr& operator=(const optional_ptr<T>&) = delete;
			
			template<class T>
			optional_ptr& operator=(optional_ptr<T>&& other) noexcept
			{
				Destroy();
				m_IsOwned = other.is_owned();
				m_Value = other.release();

				return *this;
			}

			template<class T>
			optional_ptr& operator=(std::unique_ptr<T>&& other) noexcept
			{
				reset(std::move(other));
				return *this;
			}

			template<class T>
			optional_ptr& operator=(T& other) noexcept
			{
				reset(other);
				return *this;
			}

			explicit operator bool() const noexcept
			{
				return !is_null();
			}
			bool operator!() const noexcept
			{
				return is_null();
			}
	};
}
