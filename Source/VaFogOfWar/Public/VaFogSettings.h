// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class VAFOGOFWAR_API UVaFogSettings : public UObject
{
	GENERATED_UCLASS_BODY()

	virtual void PostInitProperties() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	void SanatizeFogLayerResolution();

public:
	/** Must be power of two */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VA Fog of War Settings")
	int32 FogLayerResolution;
};
