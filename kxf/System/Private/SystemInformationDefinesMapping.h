#pragma once
#include "../SystemInformationDefines.h"
#include "kxf/Utility/Common.h"

#include <Windows.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::System::Private
{
	constexpr SystemType MapSystemType(DWORD nativeType) noexcept
	{
		switch (nativeType)
		{
			case VER_NT_SERVER:
			{
				return SystemType::Server;
			}
			case VER_NT_WORKSTATION:
			{
				return SystemType::Workstation;
			}
			case VER_NT_DOMAIN_CONTROLLER:
			{
				return SystemType::DomainController;
			}
		};
		return SystemType::Unknown;
	}
	constexpr SystemPlatformID MapSystemPlatformID(DWORD nativeType) noexcept
	{
		switch (nativeType)
		{
			case VER_PLATFORM_WIN32s:
			{
				return SystemPlatformID::Win3x;
			}
			case VER_PLATFORM_WIN32_WINDOWS:
			{
				return SystemPlatformID::Win9x;
			}
			case VER_PLATFORM_WIN32_NT:
			{
				return SystemPlatformID::WinNT;
			}
		};
		return SystemPlatformID::Unknown;
	}
	constexpr SystemProductType MapSystemProductType(DWORD nativeType) noexcept
	{
		switch (nativeType)
		{
			case PRODUCT_UNLICENSED:
			{
				return SystemProductType::Unlicensed;
			}
			case PRODUCT_BUSINESS:
			{
				return SystemProductType::Business;
			}
			case PRODUCT_BUSINESS_N:
			{
				return SystemProductType::BusinessN;
			}
			case PRODUCT_CLUSTER_SERVER:
			{
				return SystemProductType::ClusterServer;
			}
			case PRODUCT_CLUSTER_SERVER_V:
			{
				return SystemProductType::ClusterServerV;
			}
			case PRODUCT_CORE:
			{
				return SystemProductType::Core;
			}
			case PRODUCT_CORE_N:
			{
				return SystemProductType::CoreN;
			}
			case PRODUCT_CORE_COUNTRYSPECIFIC:
			{
				return SystemProductType::CoreCountrySpecific;
			}
			case PRODUCT_CORE_SINGLELANGUAGE:
			{
				return SystemProductType::CoreSingleLanguage;
			}
			case PRODUCT_DATACENTER_EVALUATION_SERVER:
			{
				return SystemProductType::DatacenterEvaluationServer;
			}
			case PRODUCT_DATACENTER_A_SERVER_CORE:
			{
				return SystemProductType::DatacenterAServerCore;
			}
			case PRODUCT_STANDARD_A_SERVER_CORE:
			{
				return SystemProductType::StandardAServerCore;
			}
			case PRODUCT_DATACENTER_SERVER:
			{
				return SystemProductType::DatacenterServer;
			}
			case PRODUCT_DATACENTER_SERVER_CORE:
			{
				return SystemProductType::DatacenterServerCore;
			}
			case PRODUCT_DATACENTER_SERVER_CORE_V:
			{
				return SystemProductType::DatacenterServerCoreV;
			}
			case PRODUCT_DATACENTER_SERVER_V:
			{
				return SystemProductType::DatacenterServerV;
			}
			case PRODUCT_EDUCATION:
			{
				return SystemProductType::Education;
			}
			case PRODUCT_EDUCATION_N:
			{
				return SystemProductType::EducationN;
			}
			case PRODUCT_ENTERPRISE:
			{
				return SystemProductType::Enterprise;
			}
			case PRODUCT_ENTERPRISE_E:
			{
				return SystemProductType::EnterpriseE;
			}
			case PRODUCT_ENTERPRISE_EVALUATION:
			{
				return SystemProductType::EnterpriseEvaluation;
			}
			case PRODUCT_ENTERPRISE_N:
			{
				return SystemProductType::EnterpriseN;
			}
			case PRODUCT_ENTERPRISE_N_EVALUATION:
			{
				return SystemProductType::EnterpriseNEvaluation;
			}
			case PRODUCT_ENTERPRISE_S:
			{
				return SystemProductType::EnterpriseS;
			}
			case PRODUCT_ENTERPRISE_S_EVALUATION:
			{
				return SystemProductType::EnterpriseSEvaluation;
			}
			case PRODUCT_ENTERPRISE_S_N:
			{
				return SystemProductType::EnterpriseSN;
			}
			case PRODUCT_ENTERPRISE_S_N_EVALUATION:
			{
				return SystemProductType::EnterpriseSNEvaluation;
			}
			case PRODUCT_ENTERPRISE_SERVER:
			{
				return SystemProductType::EnterpriseServer;
			}
			case PRODUCT_ENTERPRISE_SERVER_CORE:
			{
				return SystemProductType::EnterpriseServerCore;
			}
			case PRODUCT_ENTERPRISE_SERVER_CORE_V:
			{
				return SystemProductType::EnterpriseServerCoreV;
			}
			case PRODUCT_ENTERPRISE_SERVER_IA64:
			{
				return SystemProductType::EnterpriseServer_IA64;
			}
			case PRODUCT_ENTERPRISE_SERVER_V:
			{
				return SystemProductType::EnterpriseServerV;
			}
			case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
			{
				return SystemProductType::EssentialbusinessServerADDL;
			}
			case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
			{
				return SystemProductType::EssentialbusinessServerADDLSVC;
			}
			case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:
			{
				return SystemProductType::EssentialbusinessServerMGMT;
			}
			case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
			{
				return SystemProductType::EssentialbusinessServerMGMTSVC;
			}
			case PRODUCT_HOME_BASIC:
			{
				return SystemProductType::HomeBasic;
			}
			case PRODUCT_HOME_BASIC_E:
			{
				return SystemProductType::HomeBasicE;
			}
			case PRODUCT_HOME_BASIC_N:
			{
				return SystemProductType::HomeBasicN;
			}
			case PRODUCT_HOME_PREMIUM:
			{
				return SystemProductType::HomePremium;
			}
			case PRODUCT_HOME_PREMIUM_E:
			{
				return SystemProductType::HomePremiumE;
			}
			case PRODUCT_HOME_PREMIUM_N:
			{
				return SystemProductType::HomePremiumN;
			}
			case PRODUCT_HOME_PREMIUM_SERVER:
			{
				return SystemProductType::HomePremiumServer;
			}
			case PRODUCT_HOME_SERVER:
			{
				return SystemProductType::HomeServer;
			}
			case PRODUCT_HYPERV:
			{
				return SystemProductType::HyperV;
			}
			case PRODUCT_IOTUAP:
			{
				return SystemProductType::IoTUAP;
			}
			case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
			{
				return SystemProductType::MediumBusinessServerManagement;
			}
			case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
			{
				return SystemProductType::MediumBusinessServerMessaging;
			}
			case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
			{
				return SystemProductType::MediumBusinessServerSecurity;
			}
			case PRODUCT_MULTIPOINT_PREMIUM_SERVER:
			{
				return SystemProductType::MultipointPremiumServer;
			}
			case PRODUCT_MULTIPOINT_STANDARD_SERVER:
			{
				return SystemProductType::MultipointStandardServer;
			}
			case PRODUCT_PRO_WORKSTATION:
			{
				return SystemProductType::ProWorkstation;
			}
			case PRODUCT_PRO_WORKSTATION_N:
			{
				return SystemProductType::ProWorkstationN;
			}
			case PRODUCT_PROFESSIONAL:
			{
				return SystemProductType::Professional;
			}
			case PRODUCT_PROFESSIONAL_E:
			{
				return SystemProductType::ProfessionalE;
			}
			case PRODUCT_PROFESSIONAL_N:
			{
				return SystemProductType::ProfessionalN;
			}
			case PRODUCT_PROFESSIONAL_WMC:
			{
				return SystemProductType::ProfessionalWMC;
			}
			case PRODUCT_SB_SOLUTION_SERVER:
			{
				return SystemProductType::SbSolutionServer;
			}
			case PRODUCT_SB_SOLUTION_SERVER_EM:
			{
				return SystemProductType::SbSolutionServerEM;
			}
			case PRODUCT_SERVER_FOR_SB_SOLUTIONS:
			{
				return SystemProductType::ServerForSbSolutions;
			}
			case PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM:
			{
				return SystemProductType::ServerForSbSolutionsEM;
			}
			case PRODUCT_SERVER_FOR_SMALLBUSINESS:
			{
				return SystemProductType::ServerForSmallBusiness;
			}
			case PRODUCT_SERVER_FOR_SMALLBUSINESS_V:
			{
				return SystemProductType::ServerForSmallBusinessV;
			}
			case PRODUCT_SERVER_FOUNDATION:
			{
				return SystemProductType::ServerFoundation;
			}
			case PRODUCT_SMALLBUSINESS_SERVER:
			{
				return SystemProductType::SmallBusinessServer;
			}
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
			{
				return SystemProductType::SmallBusinessServerPremium;
			}
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:
			{
				return SystemProductType::SmallBusinessServerPremiumCore;
			}
			case PRODUCT_SOLUTION_EMBEDDEDSERVER:
			{
				return SystemProductType::SolutionEmbeddedserver;
			}
			case PRODUCT_STANDARD_EVALUATION_SERVER:
			{
				return SystemProductType::StandardEvaluationServer;
			}
			case PRODUCT_STANDARD_SERVER:
			{
				return SystemProductType::StandardServer;
			}
			case PRODUCT_STANDARD_SERVER_CORE:
			{
				return SystemProductType::StandardServerCore;
			}
			case PRODUCT_STANDARD_SERVER_CORE_V:
			{
				return SystemProductType::StandardServerCoreV;
			}
			case PRODUCT_STANDARD_SERVER_V:
			{
				return SystemProductType::StandardServerV;
			}
			case PRODUCT_STANDARD_SERVER_SOLUTIONS:
			{
				return SystemProductType::StandardServerSolutions;
			}
			case PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:
			{
				return SystemProductType::StandardServerSolutionsCore;
			}
			case PRODUCT_STARTER:
			{
				return SystemProductType::Starter;
			}
			case PRODUCT_STARTER_E:
			{
				return SystemProductType::StarterE;
			}
			case PRODUCT_STARTER_N:
			{
				return SystemProductType::StarterN;
			}
			case PRODUCT_STORAGE_ENTERPRISE_SERVER:
			{
				return SystemProductType::StorageEnterpriseServer;
			}
			case PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:
			{
				return SystemProductType::StorageEnterpriseServerCore;
			}
			case PRODUCT_STORAGE_EXPRESS_SERVER:
			{
				return SystemProductType::StorageExpressServer;
			}
			case PRODUCT_STORAGE_EXPRESS_SERVER_CORE:
			{
				return SystemProductType::StorageExpressServerCore;
			}
			case PRODUCT_STORAGE_STANDARD_EVALUATION_SERVER:
			{
				return SystemProductType::StorageStandardEvaluationServer;
			}
			case PRODUCT_STORAGE_STANDARD_SERVER:
			{
				return SystemProductType::StorageStandardServer;
			}
			case PRODUCT_STORAGE_STANDARD_SERVER_CORE:
			{
				return SystemProductType::StorageStandardServerCore;
			}
			case PRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER:
			{
				return SystemProductType::StorageWorkgroupEvaluationServer;
			}
			case PRODUCT_STORAGE_WORKGROUP_SERVER:
			{
				return SystemProductType::StorageWorkgroupServer;
			}
			case PRODUCT_STORAGE_WORKGROUP_SERVER_CORE:
			{
				return SystemProductType::StorageWorkgroupServerCore;
			}
			case PRODUCT_ULTIMATE:
			{
				return SystemProductType::Ultimate;
			}
			case PRODUCT_ULTIMATE_E:
			{
				return SystemProductType::UltimateE;
			}
			case PRODUCT_ULTIMATE_N:
			{
				return SystemProductType::UltimateN;
			}
			case PRODUCT_WEB_SERVER:
			{
				return SystemProductType::WebServer;
			}
			case PRODUCT_WEB_SERVER_CORE:
			{
				return SystemProductType::WebServerCore;
			}
		};
		return SystemProductType::Unknown;
	}
	constexpr FlagSet<SystemProductSuite> MapSystemProductSuite(DWORD nativeType) noexcept
	{
		FlagSet<SystemProductSuite> suite;
		suite.Add(SystemProductSuite::ServerNT, nativeType & VER_SERVER_NT);
		suite.Add(SystemProductSuite::WorkstationNT, nativeType & VER_WORKSTATION_NT);
		suite.Add(SystemProductSuite::Communications, nativeType & VER_SUITE_COMMUNICATIONS);
		suite.Add(SystemProductSuite::SecurityAppliance, nativeType & VER_SUITE_SECURITY_APPLIANCE);
		suite.Add(SystemProductSuite::Enterprise, nativeType & VER_SUITE_ENTERPRISE);
		suite.Add(SystemProductSuite::BackOffice, nativeType & VER_SUITE_BACKOFFICE);
		suite.Add(SystemProductSuite::Terminal, nativeType & VER_SUITE_TERMINAL);
		suite.Add(SystemProductSuite::SmallBusiness, nativeType & VER_SUITE_SMALLBUSINESS);
		suite.Add(SystemProductSuite::SmallBusinessRestricted, nativeType & VER_SUITE_SMALLBUSINESS_RESTRICTED);
		suite.Add(SystemProductSuite::EmbeddedNT, nativeType & VER_SUITE_EMBEDDEDNT);
		suite.Add(SystemProductSuite::EmbeddedRestricted, nativeType & VER_SUITE_EMBEDDED_RESTRICTED);
		suite.Add(SystemProductSuite::DataCenter, nativeType & VER_SUITE_DATACENTER);
		suite.Add(SystemProductSuite::SingleUserTS, nativeType & VER_SUITE_SINGLEUSERTS);
		suite.Add(SystemProductSuite::MultiUserTS, nativeType & VER_SUITE_MULTIUSERTS);
		suite.Add(SystemProductSuite::Personal, nativeType & VER_SUITE_PERSONAL);
		suite.Add(SystemProductSuite::Blade, nativeType & VER_SUITE_BLADE);
		suite.Add(SystemProductSuite::StorageServer, nativeType & VER_SUITE_STORAGE_SERVER);
		suite.Add(SystemProductSuite::ComputeServer, nativeType & VER_SUITE_COMPUTE_SERVER);
		suite.Add(SystemProductSuite::HomeServer, nativeType & VER_SUITE_WH_SERVER);

		return suite;
	}
}
