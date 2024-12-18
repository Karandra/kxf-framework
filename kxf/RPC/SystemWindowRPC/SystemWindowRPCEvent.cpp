#include "kxf-pch.h"
#include "SystemWindowRPCEvent.h"
#include "SystemWindowRPCServer.h"
#include "SystemWindowRPCClient.h"
#include "kxf/IO/NullStream.h"

namespace kxf
{
	void SystemWindowRPCEvent::RawSetParameters(IInputStream& stream)
	{
		m_ParametersStream = &stream;
		m_ParametersStreamOffset = stream.TellI();
	}
	MemoryInputStream SystemWindowRPCEvent::RawGetResult()
	{
		if (m_ResultStream)
		{
			return m_ResultStream.DetachStreamBuffer();
		}
		return {};
	}

	IRPCServer* SystemWindowRPCEvent::GetServer() const
	{
		return m_Server;
	}
	IRPCClient* SystemWindowRPCEvent::GetClient() const
	{
		return m_Client;
	}

	IInputStream& SystemWindowRPCEvent::RawGetParameters()
	{
		if (m_ParametersStream && m_Procedure.HasParameters())
		{
			m_ParametersStream->SeekI(m_ParametersStreamOffset, IOStreamSeek::FromStart);
			return *m_ParametersStream;
		}
		return NullInputStream::Get();
	}
	void SystemWindowRPCEvent::RawSetResult(IInputStream& stream)
	{
		m_ResultStream = {};
		if (m_Procedure.HasResult())
		{
			m_ResultStream.Write(stream);
		}
	}
}
