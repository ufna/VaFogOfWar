// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "VaFogLibrary.generated.h"

UCLASS()
class VAFOGOFWAR_API UVaFogLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Direct access to purchases controller */
	UFUNCTION(BlueprintPure, Category = "VaFog|Tools")
	static bool IsRHINeedsToSwitchVerticalAxis();
};
