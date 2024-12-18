#pragma once
#include "kxf/Sciter/Common.h"
#include "kxf/Core/OptionalPtr.h"

namespace kxf::Sciter
{
	class Host;
	class Widget;
	class Element;
	class StylesheetStorage;
}

namespace kxf::Sciter
{
	class KXF_API WidgetFactory
	{
		public:
			static std::unique_ptr<Widget> NewWidget(Host& host, const Element& element, const String& fullyQualifiedClassName);

			static Enumerator<WidgetFactory&> EnumFactories();
			static void RegisterFactory(WidgetFactory& factory);
			static void RegisterFactory(std::unique_ptr<WidgetFactory> factory);

			static StylesheetStorage& GetStylesheetStorage();

		private:
			String m_ClassName;
			String m_Namespace;

		public:
			WidgetFactory(String className, String classNamespace)
				:m_ClassName(std::move(className))
			{
			}
			virtual ~WidgetFactory() = default;

		public:
			String GetClassName() const
			{
				return m_ClassName;
			}
			String GetNamespace() const
			{
				return m_Namespace;
			}
			String GetFullyQualifiedClassName() const
			{
				if (!m_Namespace.IsEmpty())
				{
					String fullyQualifiedName;
					fullyQualifiedName.reserve(m_Namespace.length() + m_ClassName.length() + 1);

					fullyQualifiedName += m_Namespace;
					fullyQualifiedName += '.';
					fullyQualifiedName += m_ClassName;
					return fullyQualifiedName;
				}
				return m_ClassName;
			}

			bool IsStandardClass() const
			{
				return m_Namespace == "kxf";
			}

		public:
			virtual std::unique_ptr<Widget> CreateWidget(Host& host, const Element& element) = 0;
			virtual String GetWidgetStylesheet() const = 0;
	};

	class KXF_API StdWidgetFactory: public WidgetFactory
	{
		public:
			StdWidgetFactory(String className)
				:WidgetFactory(std::move(className), "kxf")
			{
			}
	};
}

namespace kxf::Sciter
{
	template<class T>
	class KXF_API WidgetFactoryInstance
	{
		private:
			inline static bool ms_IsRegistered = false;

		public:
			WidgetFactoryInstance() noexcept = default;
			WidgetFactoryInstance(const WidgetFactoryInstance&) = delete;

		protected:
			~WidgetFactoryInstance() = default;

		public:
			static T& GetInstance() noexcept(std::is_nothrow_default_constructible_v<T>)
			{
				static T factory;
				return factory;
			}
			static T& RegisterInstance()
			{
				T& factory = GetInstance();
				if (!ms_IsRegistered)
				{
					WidgetFactory::GetStylesheetStorage().AddItem(factory.GetWidgetStylesheet());
					WidgetFactory::RegisterFactory(factory);

					ms_IsRegistered = true;
				}
				return factory;
			}
			static bool IsInstanceRegistered() noexcept
			{
				return ms_IsRegistered;
			}

			static std::unique_ptr<Widget> NewWidget(Host& host, const Element& element)
			{
				return WidgetFactory::NewWidget(host, element, GetInstance().GetFullyQualifiedClassName());
			}
			static std::unique_ptr<Widget> NewWidget(Host& host, const Element& element, const String& fullyQualifiedClassName)
			{
				return WidgetFactory::NewWidget(host, element, fullyQualifiedClassName);
			}

		public:
			WidgetFactoryInstance& operator=(const WidgetFactoryInstance&) = delete;
	};
}
