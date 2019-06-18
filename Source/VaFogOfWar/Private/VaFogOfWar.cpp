// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogOfWar.h"

#include "VaFogDefines.h"
#include "VaFogSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FVaFogOfWarModule"

void FVaFogOfWarModule::StartupModule()
{
	ModuleSettings = NewObject<UVaFogSettings>(GetTransientPackage(), "VaFogSettings", RF_Standalone);
	ModuleSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "VaFogOfWar",
			LOCTEXT("RuntimeSettingsName", "VA Fog of War"),
			LOCTEXT("RuntimeSettingsDescription", "Configure VA Fog of War plugin"),
			ModuleSettings);
	}

	UE_LOG(LogVaFog, Log, TEXT("%s: VaFogOfWar module started"), *VA_FUNC_LINE);
}

void FVaFogOfWarModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "VaFogOfWar");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		ModuleSettings->RemoveFromRoot();
	}
	else
	{
		ModuleSettings = nullptr;
	}
}

UVaFogSettings* FVaFogOfWarModule::GetSettings() const
{
	check(ModuleSettings);
	return ModuleSettings;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVaFogOfWarModule, VaFogOfWar)

DEFINE_LOG_CATEGORY(LogVaFog);
