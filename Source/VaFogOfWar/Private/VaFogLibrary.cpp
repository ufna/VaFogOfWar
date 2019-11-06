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
