// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UVaFogSettings;
class UVaFogController;

/**
 * VaFogOfWar Module
 */
class FVaFogOfWarModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline FVaFogOfWarModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FVaFogOfWarModule>("VaFogOfWar");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("VaFogOfWar");
	}

	/** Getter for internal settings object to support runtime configuration changes */
	UVaFogSettings* GetSettings() const;

	/** Get global fog controller */
	UVaFogController* GetFogController(UWorld* World) const;

private:
	/** Module settings */
	UVaFogSettings* ModuleSettings;

	/** Fog controllers (one for each World we have) */
	TMap<UWorld*, UVaFogController*> FogControllers;
};
