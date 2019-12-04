// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

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
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	AVaFogLayer* Layer;

protected:
	/** Process volume bounds and apply its influence into terrain map */
	void UpdateTargetLayer();
};
