// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogLibrary.h"

#include "RHI.h"

UVaFogLibrary::UVaFogLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVaFogLibrary::IsRHINeedsToSwitchVerticalAxis()
{
	return RHINeedsToSwitchVerticalAxis(GMaxRHIShaderPlatform);
}

FColor UVaFogLibrary::GetDebugColorForHeightLevel(EVaFogHeightLevel HeightLevel)
{
	switch (HeightLevel)
	{
	case EVaFogHeightLevel::HL_1:
		return FColor::Silver;
	case EVaFogHeightLevel::HL_2:
		return FColor::Cyan;
	case EVaFogHeightLevel::HL_3:
		return FColor::Emerald;
	case EVaFogHeightLevel::HL_4:
		return FColor::Yellow;
	case EVaFogHeightLevel::HL_5:
		return FColor::Orange;
	case EVaFogHeightLevel::HL_6:
		return FColor::Magenta;
	case EVaFogHeightLevel::HL_7:
		return FColor::Purple;
	case EVaFogHeightLevel::HL_8:
		return FColor::Red;
	default:
		unimplemented();
	}

	return FColor();
}
