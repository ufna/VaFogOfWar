// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogBoundsVolume.h"

#include "VaFogController.h"
#include "VaFogDefines.h"
#include "VaFogOfWar.h"
#include "VaFogSettings.h"

#include "Components/BillboardComponent.h"
#include "Components/BrushComponent.h"
#include "Engine/CollisionProfile.h"
#include "UObject/ConstructorHelpers.h"

AVaFogBoundsVolume::AVaFogBoundsVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));

	if (!IsRunningCommandlet() && (SpriteComponent != nullptr))
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> TextRenderTexture;
			FConstructorStatics()
				: TextRenderTexture(TEXT("/Engine/EditorResources/S_SkyLight"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		SpriteComponent->Sprite = ConstructorStatics.TextRenderTexture.Get();
		SpriteComponent->RelativeScale3D = FVector(1.f, 1.f, 1.f);
		SpriteComponent->SetupAttachment(GetBrushComponent());
		SpriteComponent->bIsScreenSizeScaled = true;
		SpriteComponent->bAbsoluteScale = true;
		SpriteComponent->bReceivesDecals = false;
	}
#endif

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
		FMath::Clamp(FMath::RoundToInt(LayerPosition.Y) + LayerToTextureShift, 0, CachedFogLayerResolution - 1),
		FMath::Clamp(LayerToTextureShift - FMath::RoundToInt(LayerPosition.X), 0, CachedFogLayerResolution - 1));
}

int32 AVaFogBoundsVolume::ScaleDistanceToLayer(const int32 Distance) const
{
	// For now we assume that volume is square and use X only
	return Distance / VolumeTransform.GetScale3D().X;
}
