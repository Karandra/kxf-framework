#include "KxfPCH.h"
#include "BasicWebSession.h"
#include "BasicWebRequest.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/Utility/Container.h"

namespace kxf::Private
{
	bool BasicWebSession::SetHeader(std::vector<WebRequestHeader>& headerStorage, const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags)
	{
		if (header && flags)
		{
			auto it = Utility::Container::FindIf(headerStorage, [&](const WebRequestHeader& item)
			{
				return item.IsSameAs(header);
			});

			if (it != headerStorage.end())
			{
				if (header.IsEmpty())
				{
					if (flags.Contains(WebRequestHeaderFlag::Replace))
					{
						headerStorage.erase(it);
						return true;
					}
				}
				else
				{
					if (flags.Contains(WebRequestHeaderFlag::Add))
					{
						if (flags.Contains(WebRequestHeaderFlag::CoalesceComma))
						{
							it->AddValue(header.GetValue(), WebRequestHeaderFlag::CoalesceComma);
							return true;
						}
						else if (flags.Contains(WebRequestHeaderFlag::CoalesceSemicolon))
						{
							it->AddValue(header.GetValue(), WebRequestHeaderFlag::CoalesceSemicolon);
							return true;
						}
					}
					else if (flags.Contains(WebRequestHeaderFlag::Replace))
					{
						*it = header;
						return true;
					}
				}
			}
			else if (!header.IsEmpty())
			{
				if (flags.Contains(WebRequestHeaderFlag::Add))
				{
					headerStorage.emplace_back(header);
					return true;
				}
			}
			return false;
		}
		return false;
	}

	void BasicWebSession::DoInitialize(std::shared_ptr<IAsyncTaskExecutor> taskExecutor)
	{
		if (taskExecutor)
		{
			m_TaskExecutor = std::move(taskExecutor);
		}
		else
		{
			m_TaskExecutor = RTTI::assume_non_owned(ICoreApplication::GetInstance()->GetTaskExecutor());
		}
	}

	bool BasicWebSession::IsNull() const noexcept
	{
		return m_TaskExecutor == nullptr;
	}
	bool BasicWebSession::StartRequest(BasicWebRequest& request)
	{
		if (auto locked = request.LockRef())
		{
			m_TaskExecutor->QueueTask([request = std::move(locked)]()
			{
				request->PerformRequest();
			});
			return true;
		}
		return false;
	}
}
