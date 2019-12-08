// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "VaFogTypes.h"

#include "VaFogLibrary.generated.h"

UCLASS()
class VAFOGOFWAR_API UVaFogLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** For mobile platforms that switch vertical axis source buffer should be flipped */
	UFUNCTION(BlueprintPure, Category = "VaFog|Tools")
	static bool IsRHINeedsToSwitchVerticalAxis();

	/** Used for debug quads colorings */
	static FColor GetDebugColorForHeightLevel(EVaFogHeightLevel HeightLevel);
};
