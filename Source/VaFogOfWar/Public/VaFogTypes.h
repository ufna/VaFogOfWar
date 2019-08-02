// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.generated.h"

UENUM(BlueprintType)
enum class EVaFogLayerChannel : uint8
{
	/** "Black" fog of war */
	Permanent,
	/** "Grey" fog of war */
	Scouting
};

UENUM(BlueprintType)
enum class EVaFogAgentType : uint8
{
	/** Fog dispeller (scout) */
	Dispel,
	/** Visibility blocker */
	Obstacle
};
