#pragma once
#include "Common.h"

namespace kxf
{
	class IEnumerator;
}

namespace kxf
{
	template<class TValue_, class TPath_>
	class RecursiveCollectionEnumerator
	{
		public:
			using TValue = TValue_;
			using TPath = TPath_;

		private:
			TPath m_RootPath;

			std::vector<TPath> m_SubDirectories;
			bool m_SubTreeDone = false;

			std::vector<TPath> m_NextSubDirectories;
			size_t m_NextSubDirectory = 0;
			bool m_NextSubTreeDone = false;

		private:
			std::optional<TValue> MoveNext(IEnumerator& enumerator)
			{
				if (m_RootPath)
				{
					if (!m_SubTreeDone)
					{
						// Do the root level
						return SearchDirectory(enumerator, m_RootPath, m_SubDirectories, m_SubTreeDone);
					}
					else if (!m_SubDirectories.empty())
					{
						// Once we have subdirectories to scan get the one pointed by 'm_NextSubDirectory' and scan it
						// placing any subsequent subdirectories inside the container. Do that until we have scanned
						// all subdirectories for the previous level or an end was signaled (via 'm_NextSubTreeDone' flag).
						if (m_NextSubDirectory < m_SubDirectories.size() && !m_NextSubTreeDone)
						{
							auto item = SearchDirectory(enumerator, std::move(m_SubDirectories[m_NextSubDirectory]), m_NextSubDirectories, m_NextSubTreeDone);

							// Advance to the next directory in the list or break out and go level down
							if (m_NextSubTreeDone)
							{
								if (++m_NextSubDirectory < m_SubDirectories.size())
								{
									// Reset the flag to scan the next directory on the current level
									m_NextSubTreeDone = false;
								}
								else
								{
									// Set the flag (or leave it unchanged rather) to break out and process deeper levels
									m_NextSubTreeDone = true;
								}
							}
							return item;
						}
						else
						{
							// When we're done with the second level move its subdirectories to the base level container and
							// reset its done flag.
							m_SubDirectories = std::move(m_NextSubDirectories);
							m_SubTreeDone = true;

							// Also reset the second level state
							m_NextSubDirectory = 0;
							m_NextSubTreeDone = false;

							if (!m_SubDirectories.empty())
							{
								// Skip this iteration if we need to get back to base level with subdirectories from the second level
								enumerator.SkipCurrent();
							}
						}
					}
				}
				return {};
			}

		protected:
			virtual std::optional<TValue> SearchDirectory(IEnumerator& enumerator, const TPath& directory, std::vector<TPath>& childDirectories, bool& isSubTreeDone) = 0;

		public:
			RecursiveCollectionEnumerator() = default;
			RecursiveCollectionEnumerator(TPath rootPath)
				:m_RootPath(std::move(rootPath))
			{
			}
			virtual ~RecursiveCollectionEnumerator() = default;

		public:
			std::optional<TValue> operator()(IEnumerator& enumerator)
			{
				return MoveNext(enumerator);
			}
			const TPath& GetRootPath() const
			{
				return m_RootPath;
			}
	};
}
