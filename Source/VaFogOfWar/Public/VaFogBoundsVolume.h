// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"

#include "VaFogBoundsVolume.generated.h"

UCLASS()
class VAFOGOFWAR_API AVaFogBoundsVolume : public AVolume
{
	GENERATED_UCLASS_BODY()

	//~ Begin AActor Interface
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;
	//~ End AActor Interface

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UBillboardComponent* SpriteComponent;
#endif

public:
	/** Convert world location of vector to layer one */
	FIntPoint TransformWorldToLayer(const FVector& AgentLocation) const;

	/** Convert distance in wolrd units to the layer one */
	int32 ScaleDistanceToLayer(const int32 Distance) const;

private:
	int32 CachedFogLayerResolution;

	/** Shift that should be applied to transform layer coordinates into texture coordinates */
	int32 LayerToTextureShift;

	/** World to layer transform */
	FTransform VolumeTransform;
};
