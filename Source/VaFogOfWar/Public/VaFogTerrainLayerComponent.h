// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogLayerComponent.h"

#include "VaFogTerrainLayerComponent.generated.h"

/** 
 * Pre-configured layer component for handling terrain and obstacles
 */
UCLASS(ClassGroup = (VAFogOfWar), editinlinenew, meta = (BlueprintSpawnableComponent))
class VAFOGOFWAR_API UVaFogTerrainLayerComponent : public UVaFogLayerComponent
{
	GENERATED_UCLASS_BODY()
};
