// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.h"

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"

#include "VaFogBlockingVolume.generated.h"

class AVaFogLayer;

UCLASS()
class VAFOGOFWAR_API AVaFogBlockingVolume : public AVolume
{
	GENERATED_UCLASS_BODY()

	//~ Begin AActor Interface
	virtual void PostLoad() override;
	virtual void PostActorCreated() override;
	virtual void OnConstruction(const FTransform& Transform) override;
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
	/** Target layer to paint on */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "VaFog Blocking Volume")
	AVaFogLayer* Layer;

	/** Height level where agent is standing */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VaFog Blocking Volume")
	EVaFogHeightLevel HeightLevel;

	/** Volumes will affect terrain level being sorted by Priority */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "VaFog Blocking Volume")
	int32 Priority;

protected:
	/** Process volume bounds and apply its influence into terrain map */
	void UpdateTargetLayer();

	//////////////////////////////////////////////////////////////////////////
	// Debug

public:
	/** Debug fog layer affect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDebugVolume;
};
