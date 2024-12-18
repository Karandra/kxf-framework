#include "kxf-pch.h"
#include "CURLWebAuthChallenge.h"
#include "CURLWebRequest.h"

namespace kxf
{
	bool CURLWebAuthChallenge::SetCredentials(UserCredentials credentials)
	{
		return m_Request.OnSetAuthChallengeCredentials(m_Source, std::move(credentials));
	}
}
