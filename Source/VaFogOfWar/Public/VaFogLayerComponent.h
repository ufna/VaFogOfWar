// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.h"

#include "Components/ActorComponent.h"

#include "VaFogLayerComponent.generated.h"

class UTextureRenderTarget2D;
class UMaterialInterface;

UCLASS(ClassGroup = (VAFogOfWar), editinlinenew, meta = (BlueprintSpawnableComponent))
class VAFOGOFWAR_API UVaFogLayerComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	/**  */
	UPROPERTY(EditDefaultsOnly, Category = "Fog of War")
	EVaFogLayerChannel LayerChannel;

	/**  */
	UPROPERTY(EditDefaultsOnly, Category = "Fog of War")
	UTextureRenderTarget2D* OriginalRenderTarget;

	/**  */
	UPROPERTY(EditDefaultsOnly, Category = "Fog of War")
	UTextureRenderTarget2D* FinalRenderTarget;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fog of War")
	UMaterialInterface* BlurMaterial;
};
