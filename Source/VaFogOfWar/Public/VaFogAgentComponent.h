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

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnRegister() override;
#endif

public:
	/** Set agent enabled (or disable it) */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Agent")
	void EnableAgent(bool bEnable = true);

	/** Set agent disabled */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Agent")
	void DisableAgent();

	/** Check agent status */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Agent")
	bool IsAgentEnabled() const;

	/** Set agent new vision radius */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Agent")
	void SetVisionRadius(int32 NewVisionRadius);

	/** Set agent new height level */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Agent")
	void SetHeightLevel(EVaFogHeightLevel NewHeightLevel);

public:
	/** Is agent is enabled by default? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VaFog Agent")
	bool bAgentEnabled;

	/** Fog layer we're scouting by the agent */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VaFog Agent")
	TSet<EVaFogLayerChannel> TargetChannels;

	/** Radius strategy used for fog interaction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VaFog Agent")
	EVaFogRadiusStrategy RadiusStrategy;

	/** Agent vision radius in cm (set 0 to use single cell vision) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VaFog Agent")
	int32 VisionRadius;

	/** Height level where agent is standing */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VaFog Agent")
	EVaFogHeightLevel HeightLevel;

private:
	/** Update agent status with Fog Controller */
	void UpdateAgentRegistration();

#if WITH_EDITORONLY_DATA
	/** Utility function that updates which texture is displayed on the sprite dependent on the properties of the Component. */
	void UpdateSpriteTexture();
#endif
};
