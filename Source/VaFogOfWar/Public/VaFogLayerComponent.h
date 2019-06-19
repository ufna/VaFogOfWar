// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.h"

#include "Components/ActorComponent.h"

#include "VaFogLayerComponent.generated.h"

class UTextureRenderTarget2D;
class UMaterialInterface;

class UVaFogAgentComponent;

UCLASS(ClassGroup = (VAFogOfWar), editinlinenew, meta = (BlueprintSpawnableComponent))
class VAFOGOFWAR_API UVaFogLayerComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddFogAgent(UVaFogAgentComponent* InFogAgent);
	void RemoveFogAgent(UVaFogAgentComponent* InFogAgent);

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

protected:
	/** Registered fog agents for layer */
	UPROPERTY()
	TArray<UVaFogAgentComponent*> FogAgents;
};
