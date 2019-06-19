// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogBoundsVolume.h"

#include "VaFogController.h"
#include "VaFogDefines.h"
#include "VaFogOfWar.h"
#include "VaFogSettings.h"

#include "Components/BrushComponent.h"
#include "Engine/CollisionProfile.h"
#include "Builders/CubeBuilder.h"

AVaFogBoundsVolume::AVaFogBoundsVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->Mobility = EComponentMobility::Static;

	BrushColor = FColor(200, 0, 0, 255);
	bColored = true;

	CachedFogLayerResolution = 128;
}

void AVaFogBoundsVolume::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	// Cache layers resolution for coordinates transform
	CachedFogLayerResolution = FVaFogOfWarModule::Get().GetSettings()->FogLayerResolution;
	check(FMath::IsPowerOfTwo(CachedFogLayerResolution));

	// Calculate world to layet transform
	FVector BoxExtent = GetBrushComponent()->Bounds.BoxExtent;
	//GetBrushComponent()->Bounds.Origin
	UE_LOG(LogVaFog, Warning, TEXT("[%s] BoxExtent: %s"), *VA_FUNC_LINE, *BoxExtent.ToCompactString());

	/*VolumeTransform.SetLocation();
	VolumeTransform.SetRotation();
	VolumeTransform.SetScale3D();*/

	UVaFogController::Get(this)->OnFogBoundsAdded(this);
}

void AVaFogBoundsVolume::Destroyed()
{
	Super::Destroyed();

	UVaFogController::Get(this)->OnFogBoundsRemoved(this);
}

#if WITH_EDITOR
void AVaFogBoundsVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// @TODO Force volume brush to be square or use custom BrushBuilder
}
#endif

FIntPoint AVaFogBoundsVolume::TransformWorldToLayer(const FVector& AgentLocation) const
{
	return FIntPoint();
}
