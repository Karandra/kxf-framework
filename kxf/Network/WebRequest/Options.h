#pragma once
#include "Common.h"

// Undef these symbols because Windows SDK defines some of them
#undef OPTIONS
#undef GET
#undef HEAD
#undef POST
#undef PUT
#undef DELETE
#undef TRACE
#undef CONNECT

namespace kxf
{
	enum class WebRequestOption2
	{
		Disabled = 0,
		Enabled = 1
	};
	enum class WebRequestOption3
	{
		Diabled,
		Enabled,
		Required
	};
	enum class WebRequestMethod
	{
		None = -1,

		OPTIONS,
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		TRACE,
		CONNECT,
		PATCH
	};
	enum class WebRequestSSLVersion
	{
		Default = -1,

		SSLv2,
		SSLv3,

		TLSv1_x,
		TLSv1_0,
		TLSv1_1,
		TLSv1_2,
		TLSv1_3
	};

	enum class WebRequestAuthMethod: uint32_t
	{
		None = 0,

		Basic = 1 << 0,
		NTLM = 1 << 1,
		Digest = 1 << 2,
		Bearer = 1 << 3,
		Negotiate = 1 << 4
	};
	kxf_FlagSet_Declare(WebRequestAuthMethod);

	enum class WebRequestIPVersion
	{
		Any = -2,
		None = -1,

		IPv4,
		IPv6
	};
	enum class WebRequestHTTPVersion
	{
		Any = -2,
		None = -1,

		Version1_0,
		Version1_1,
		Version2,
		Version2TLS,
		Version3
	};

	enum class WebRequestProtocol: uint64_t
	{
		None = 0,

		DICT = FlagSetValue<WebRequestProtocol>(0),
		FILE = FlagSetValue<WebRequestProtocol>(1),
		FTP = FlagSetValue<WebRequestProtocol>(2),
		FTPS = FlagSetValue<WebRequestProtocol>(3),
		GOPHER = FlagSetValue<WebRequestProtocol>(4),
		HTTP = FlagSetValue<WebRequestProtocol>(5),
		HTTPS = FlagSetValue<WebRequestProtocol>(6),
		IMAP = FlagSetValue<WebRequestProtocol>(7),
		IMAPS = FlagSetValue<WebRequestProtocol>(8),
		LDAP = FlagSetValue<WebRequestProtocol>(9),
		LDAPS = FlagSetValue<WebRequestProtocol>(10),
		POP3 = FlagSetValue<WebRequestProtocol>(11),
		POP3S = FlagSetValue<WebRequestProtocol>(12),
		RTMP = FlagSetValue<WebRequestProtocol>(13),
		RTMPE = FlagSetValue<WebRequestProtocol>(14),
		RTMPS = FlagSetValue<WebRequestProtocol>(15),
		RTMPT = FlagSetValue<WebRequestProtocol>(16),
		RTMPTE = FlagSetValue<WebRequestProtocol>(17),
		RTMPTS = FlagSetValue<WebRequestProtocol>(18),
		RTSP = FlagSetValue<WebRequestProtocol>(19),
		SCP = FlagSetValue<WebRequestProtocol>(20),
		SFTP = FlagSetValue<WebRequestProtocol>(21),
		SMB = FlagSetValue<WebRequestProtocol>(22),
		SMBS = FlagSetValue<WebRequestProtocol>(23),
		SMTP = FlagSetValue<WebRequestProtocol>(24),
		SMTPS = FlagSetValue<WebRequestProtocol>(25),
		TELNET = FlagSetValue<WebRequestProtocol>(26),
		TFTP = FlagSetValue<WebRequestProtocol>(27),
		WS = FlagSetValue<WebRequestProtocol>(28),
		WSS = FlagSetValue<WebRequestProtocol>(29),

		Everything = std::numeric_limits<uint64_t>::max()
	};
	kxf_FlagSet_Declare(WebRequestProtocol);
}
