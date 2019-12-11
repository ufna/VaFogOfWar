// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogBoundsVolume.h"

#include "VaFogController.h"
#include "VaFogDefines.h"
#include "VaFogOfWar.h"
#include "VaFogSettings.h"

#include "Components/BillboardComponent.h"
#include "Components/BrushComponent.h"
#include "DrawDebugHelpers.h"
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
		SpriteComponent->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
		SpriteComponent->SetupAttachment(GetBrushComponent());
		SpriteComponent->bIsScreenSizeScaled = true;
		SpriteComponent->SetUsingAbsoluteScale(true);
		SpriteComponent->bReceivesDecals = false;
	}
#endif
#if WITH_EDITORONLY_DATA
	PrimaryActorTick.bCanEverTick = true;
#endif

	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->Mobility = EComponentMobility::Static;

	BrushColor = FColor(200, 0, 0, 255);
	bColored = true;

	CachedFogLayerResolution = 128;
	LayerToTextureShift = 64;

	DebugTime = 20.f;
}

void AVaFogBoundsVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateVolumeTransform();
}

void AVaFogBoundsVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UpdateVolumeTransform();
}

void AVaFogBoundsVolume::Destroyed()
{
	Super::Destroyed();
}

bool AVaFogBoundsVolume::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AVaFogBoundsVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDebugVolume)
	{
		DrawDebugGrid();

		// It's really not good idea to update it each tick
		bDebugVolume = false;
	}
}

#if WITH_EDITOR
void AVaFogBoundsVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UpdateVolumeTransform();

	// @TODO Force volume brush to be square or use custom BrushBuilder

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AVaFogBoundsVolume::UpdateVolumeTransform()
{
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

	// Calculate and cache cell extent
	float CellExtentX = GetBrushComponent()->Bounds.BoxExtent.X / CachedFogLayerResolution;
	float CellExtentY = GetBrushComponent()->Bounds.BoxExtent.Y / CachedFogLayerResolution;
	CachedCellExtent = FVector(CellExtentX, CellExtentY, 0.f);
}

FVector AVaFogBoundsVolume::GetCellExtent() const
{
	return CachedCellExtent;
}

FIntPoint AVaFogBoundsVolume::TransformWorldToLayer(const FVector& AgentLocation) const
{
	// First transform position into volume local coordinates (scaled with fog rt resolution)
	FVector LayerPosition = VolumeTransform.InverseTransformPosition(AgentLocation);

	// Then transform it into texture coordinates
	return FIntPoint(
		FMath::Clamp(FMath::FloorToInt(LayerPosition.Y) + LayerToTextureShift, 0, CachedFogLayerResolution - 1),
		FMath::Clamp(LayerToTextureShift - FMath::CeilToInt(LayerPosition.X), 0, CachedFogLayerResolution - 1));
}

int32 AVaFogBoundsVolume::ScaleDistanceToLayer(const int32 Distance) const
{
	// For now we assume that volume is square and use X only
	return Distance / VolumeTransform.GetScale3D().X;
}

void AVaFogBoundsVolume::DrawDebugGrid()
{
	FIntPoint LayerPoint;
	FVector LayerPosition = FVector::ZeroVector;
	int32 LocationZ = GetActorLocation().Z;

	for (int32 i = 0; i < CachedFogLayerResolution; i++)
	{
		for (int32 j = 0; j < CachedFogLayerResolution; j++)
		{
			LayerPosition.X = i - LayerToTextureShift;
			LayerPosition.Y = j - LayerToTextureShift + 1;
			LayerPosition.Z = 0;

			LayerPosition = VolumeTransform.TransformPosition(LayerPosition);
			LayerPosition += GetCellExtent();
			LayerPosition.Z = LocationZ;

			DrawDebugBox(GetWorld(), LayerPosition, GetCellExtent() * 0.95f, GetTransform().GetRotation(), FColor::Green, false, DebugTime);
		}
	}
}

FVector AVaFogBoundsVolume::SnapWorldToGrid(const FVector& InLocation) const
{
	FIntPoint GridPoint = TransformWorldToLayer(InLocation) + FIntPoint(-LayerToTextureShift, 0);
	return VolumeTransform.TransformPosition(FVector(LayerToTextureShift - GridPoint.Y, GridPoint.X, 0.f));
}
