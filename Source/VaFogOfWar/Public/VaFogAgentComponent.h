// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.h"

#include "Components/SceneComponent.h"

#include "VaFogAgentComponent.generated.h"

UCLASS(ClassGroup = (VAFogOfWar), editinlinenew, meta = (BlueprintSpawnableComponent))
class VAFOGOFWAR_API UVaFogAgentComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

public:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnRegister() override;
#endif

public:
	/** Fog layer we're scouting by the agent */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EVaFogLayerChannel TargetChannel;

	/** Type of fog channel interaction  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EVaFogAgentType InteractionType;

	/** Radius strategy used for fog interaction 
	 * @TODO Make radius strategies configurable https://github.com/ufna/VaFogOfWar/issues/58 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EVaFogRadiusStrategy RadiusStrategy;

	/** Agent vision radius in cm (set 0 to use single cell vision) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 VisionRadius;

private:
#if WITH_EDITORONLY_DATA
	/** Utility function that updates which texture is displayed on the sprite dependent on the properties of the Component. */
	void UpdateSpriteTexture();
#endif
};
