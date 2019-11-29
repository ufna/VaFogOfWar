// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogLayer.h"

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"

#include "VaFogBlockingVolume.generated.h"

UCLASS()
class VAFOGOFWAR_API AVaFogBlockingVolume : public AVolume
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
	UPROPERTY(EditAnywhere)
	FComponentReference Layer;

protected:
	/** Process volume bounds and apply its influence into terrain map */
	void UpdateVolumeLayers();
};
