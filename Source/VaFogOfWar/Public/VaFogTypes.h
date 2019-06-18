// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.generated.h"

UENUM(BlueprintType)
enum class EVaFogLayerChannel : uint8
{
	/** "Black" fog of war */
	World,
	/** "Grey" fog of war */
	Reconnaissance
};
