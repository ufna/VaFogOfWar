// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.generated.h"

UENUM(BlueprintType)
enum class EVaFogLayerChannel : uint8
{
	/** "Black" fog of war */
	Permanent,
	/** "Grey" fog of war */
	Scouting,
	/** Vision obstacles */
	Terrain
};

UENUM(BlueprintType)
enum class EVaFogRadiusStrategy : uint8
{
	Circle,
	Square,
	SquareStepped, // Radius treated as diameter (used for obstalces mostly)

	Max UMETA(Hidden)
};

/** 
 * There are six different height levels that have gameplay relevance. One height level can be connected to another by a stair (passable) or a cliff (obstructed).
 */
// clang-format off
UENUM(BlueprintType)
enum class EVaFogHeightLevel : uint8
{
	HL_INVALID = 0 UMETA(Hidden),
	HL_1 = 0x01 UMETA(Hidden),
	HL_2 = 0x02 UMETA(DisplayName = "River"),	   // River
	HL_3 = 0x04 UMETA(DisplayName = "Lowground"),  // Lowground
	HL_4 = 0x08 UMETA(DisplayName = "Highground"), // Highground
	HL_5 = 0x10 UMETA(DisplayName = "Elevated"),   // Elevated
	HL_6 = 0x20 UMETA(DisplayName = "Cliffs"),	   // Cliffs
	HL_7 = 0x40 UMETA(DisplayName = "Valley"),	   // Valley
	HL_8 = 0x80 UMETA(Hidden)
};
// clang-format on
