#include "kxf-pch.h"
#include "MemoryStream.h"

namespace kxf
{
	MemoryInputStream::MemoryInputStream(const MemoryOutputStream& stream) noexcept
	{
		auto& buffer = stream.GetStreamBuffer();
		m_StreamBuffer.AttachStorage(buffer.GetBufferStart(), buffer.GetBufferEnd());
		m_StreamBuffer.SetStorageFixed();
	}
	MemoryInputStream::MemoryInputStream(MemoryOutputStream&& stream) noexcept
		:m_StreamBuffer(std::move(stream.GetStreamBuffer()))
	{
	}
}

namespace kxf
{
	// IReadableOutputStream
	std::unique_ptr<IInputStream> MemoryOutputStream::CreateInputStream() const
	{
		return std::make_unique<MemoryInputStream>(m_StreamBuffer.GetBufferStart(), m_StreamBuffer.GetBufferEnd());
	}
}
