// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogBoundsVolume.h"

#include "VaFogController.h"
#include "VaFogDefines.h"

#include "Components/BrushComponent.h"
#include "Engine/CollisionProfile.h"

AVaFogBoundsVolume::AVaFogBoundsVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->Mobility = EComponentMobility::Static;

	BrushColor = FColor(200, 0, 0, 255);
	bColored = true;
}

void AVaFogBoundsVolume::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	UVaFogController::Get(this)->OnFogBoundsAdded(this);
}

void AVaFogBoundsVolume::PostUnregisterAllComponents()
{
	Super::PostUnregisterAllComponents();

	// World can be cleanuped already at this point
	if (UVaFogController::Get(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		UVaFogController::Get(this)->OnFogBoundsRemoved(this);
	}
}