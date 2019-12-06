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
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;

	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void Tick(float DeltaTime) override;
	//~ End AActor Interface

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UBillboardComponent* SpriteComponent;
#endif

protected:
	/** Cache volume transform from scene and fog grid resolution from settings */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Bounds")
	void UpdateVolumeTransform();

public:
	/** Get fog cell size in units */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Bounds")
	FVector GetCellExtent() const;

	/** Convert world location of vector to layer one */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Bounds")
	FIntPoint TransformWorldToLayer(const FVector& AgentLocation) const;

	/** Convert distance in wolrd units to the layer one */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Bounds")
	int32 ScaleDistanceToLayer(const int32 Distance) const;

private:
	int32 CachedFogLayerResolution;
	FVector CachedCellExtent;

	/** Shift that should be applied to transform layer coordinates into texture coordinates */
	int32 LayerToTextureShift;

	/** World to layer transform */
	FTransform VolumeTransform;

	//////////////////////////////////////////////////////////////////////////
	// Debug

protected:
	/** Show grid for this layer */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDebugVolume;

	/** Time to show debug grid */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	float DebugTime;

protected:
	void DrawDebugGrid();

public:
	/** Get nearest cell center */
	FVector SnapWorldToGrid(const FVector& InLocation) const;
};
