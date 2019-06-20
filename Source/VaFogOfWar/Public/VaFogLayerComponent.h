// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.h"

#include "Components/ActorComponent.h"

#include "VaFogLayerComponent.generated.h"

class UTextureRenderTarget2D;
class UTexture2D;
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
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Fog of War")
	UTexture2D* OriginalTexture;

	/** Updated original texture region */
	FUpdateTextureRegion2D OriginalRegion;

	/** Original layer texture on CPU */
	uint8* OriginalBuffer;

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

private:
	int32 CachedTextureResolution;
	int32 W;
	int32 H;
	int32 OriginalBufferLength;

	//////////////////////////////////////////////////////////////////////////
	// Debug

public:
	/** Show agents vision radius for this layer */
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bDebugAgents;

	/** Color to draw */
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	FColor DebugAgentsColor;
};
