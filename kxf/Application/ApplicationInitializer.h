#pragma once
#include "Common.h"
#include "ICoreApplication.h"
#include "kxf/System/Common.h"

// This class can be used to perform full initialize-run-shutdown sequence of the application.
// Use as shown:
/*
	int main(int argc, char** argv)
	{
		MyAppNamespace::MyApp app;
		return kxf::ApplicationInitializer(app, argc, argv).Run();
	}
*/

// Useful MSVC linker config when using this method:
/*
	/SUBSYSTEM:windows
	/ENTRY:mainCRTStartup
*/

namespace kxf::Application::Private
{
	class NativeApp;
}

namespace kxf
{
	class KXF_API ApplicationInitializer final
	{
		private:
			std::unique_ptr<Application::Private::NativeApp> m_NativeApp;

			ICoreApplication& m_Application;
			XChar** m_CommandLine = nullptr;
			int m_CommandLineCount = 0;

			bool m_IsInitializedCommon = false;
			bool m_IsInitialized = false;
			bool m_IsCreated = false;

		private:
			bool OnInitCommon();
			bool OnInit();
			bool OnInit(int argc, char** argv);
			bool OnInit(int argc, wchar_t** argv);
			void OnInitDone();

			template<class... Args>
			void RunInitSequence(Args&&... arg)
			{
				if (m_IsInitializedCommon = OnInitCommon())
				{
					if (m_IsCreated = m_Application.OnCreate())
					{
						if (m_IsInitialized = OnInit(std::forward<Args>(arg)...))
						{
							OnInitDone();
						}
					}
				}
			}

			void OnTerminate();

		public:
			ApplicationInitializer(ICoreApplication& app);
			ApplicationInitializer(ICoreApplication& app, int argc, char** argv);
			ApplicationInitializer(ICoreApplication& app, int argc, wchar_t** argv);
			~ApplicationInitializer();

		public:
			bool IsInitialized() const noexcept
			{
				return m_IsInitializedCommon && m_IsInitialized;
			}
			bool IsCreated() const noexcept
			{
				return m_IsCreated;
			}

			int Run() noexcept;
	};
}
