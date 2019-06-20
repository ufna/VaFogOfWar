// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogBoundsVolume.h"

#include "VaFogController.h"
#include "VaFogDefines.h"
#include "VaFogOfWar.h"
#include "VaFogSettings.h"

#include "Components/BrushComponent.h"
#include "Engine/CollisionProfile.h"

AVaFogBoundsVolume::AVaFogBoundsVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->Mobility = EComponentMobility::Static;

	BrushColor = FColor(200, 0, 0, 255);
	bColored = true;

	CachedFogLayerResolution = 128;
	LayerToTextureShift = 64;
}

void AVaFogBoundsVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Cache layers resolution for coordinates transform
	CachedFogLayerResolution = FVaFogOfWarModule::Get().GetSettings()->FogLayerResolution;
	check(FMath::IsPowerOfTwo(CachedFogLayerResolution));
	LayerToTextureShift = CachedFogLayerResolution / 2;

	// Calculate world to layet transform
	float VolumeScaleX = (GetBrushComponent()->Bounds.BoxExtent.X * 2) / CachedFogLayerResolution;
	float VolumeScaleY = (GetBrushComponent()->Bounds.BoxExtent.Y * 2) / CachedFogLayerResolution;
	VolumeTransform.SetScale3D({VolumeScaleX, VolumeScaleY, 1});

	VolumeTransform.SetRotation(GetBrushComponent()->GetComponentTransform().GetRotation());
	VolumeTransform.SetLocation(GetBrushComponent()->GetComponentTransform().GetLocation());

	UE_LOG(LogVaFog, Warning, TEXT("[%s] Cache VolumeTransform: \n%s"), *VA_FUNC_LINE, *VolumeTransform.ToHumanReadableString());

	if (UVaFogController::Get(this))
	{
		UVaFogController::Get(this)->OnFogBoundsAdded(this);
	}
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
	// First transform position into volume local coordinates (scaled with fog rt resolution)
	FVector LayerPosition = VolumeTransform.InverseTransformPosition(AgentLocation);

	// Then transform it into texture coordinates
	return FIntPoint(
		FMath::Clamp(FMath::CeilToInt(LayerPosition.Y) + LayerToTextureShift, 0, CachedFogLayerResolution - 1),
		FMath::Clamp(LayerToTextureShift - FMath::CeilToInt(LayerPosition.X), 0, CachedFogLayerResolution - 1));
}
