// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogOfWar.h"

#include "VaFogController.h"
#include "VaFogDefines.h"
#include "VaFogSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Engine/World.h"

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

	FWorldDelegates::OnPostWorldCleanup.AddLambda([this](UWorld* World, bool bSessionEnded, bool bCleanupResources) {
		FogControllers.Remove(World);

		UE_LOG(LogVaFog, Log, TEXT("[%s] Fog Controller is removed for: %s"), *VA_FUNC_LINE, *World->GetName());
	});

	FWorldDelegates::OnPostWorldInitialization.AddLambda([this](UWorld* World, const UWorld::InitializationValues IVS) {
		auto FogController = NewObject<UVaFogController>(GetTransientPackage());
		FogController->SetFlags(RF_Standalone);
		FogController->AddToRoot();

		FogControllers.Add(World, FogController);

		UE_LOG(LogVaFog, Log, TEXT("[%s] Fog Controller is created for: %s"), *VA_FUNC_LINE, *World->GetName());
	});

	UE_LOG(LogVaFog, Log, TEXT("[%s] VaFogOfWar module started"), *VA_FUNC_LINE);
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

		for (auto FogController : FogControllers)
		{
			FogController.Value->RemoveFromRoot();
		}
	}
	else
	{
		ModuleSettings = nullptr;
	}

	FogControllers.Empty();
}

UVaFogSettings* FVaFogOfWarModule::GetSettings() const
{
	check(ModuleSettings);
	return ModuleSettings;
}

UVaFogController* FVaFogOfWarModule::GetFogController(UWorld* World) const
{
	return FogControllers.FindRef(World);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVaFogOfWarModule, VaFogOfWar)

DEFINE_LOG_CATEGORY(LogVaFog);
