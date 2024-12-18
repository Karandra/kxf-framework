#include <cstring>
#include <type_traits>

#ifdef ZeroMemory
#undef ZeroMemory
inline void* ZeroMemory(void* ptr, size_t size) noexcept
{
	return std::memset(ptr, 0, size);
}
#endif

#ifdef CopyMemory
#undef CopyMemory
inline void* CopyMemory(void* dst, const void* src, size_t size) noexcept
{
	return std::memcpy(dst, src, size);
}
#endif

#ifdef MoveMemory
#undef MoveMemory
inline void* MoveMemory(void* dst, const void* src, size_t size) noexcept
{
	return std::memmove(dst, src, size);
}
#endif

#ifdef FillMemory
#undef FillMemory
inline void* FillMemory(void* dst, size_t size, int fill) noexcept
{
	return std::memset(dst, fill, size);
}
#endif

#ifdef EqualMemory
#undef EqualMemory
inline bool EqualMemory(const void* left, const void* right, size_t size) noexcept
{
	return std::memcmp(left, right, size) == 0;
}
#endif

#ifdef SecureZeroMemory
#undef SecureZeroMemory
inline void* SecureZeroMemory(void* ptr, size_t size) noexcept
{
	return ::RtlSecureZeroMemory(ptr, size);
}
#endif
