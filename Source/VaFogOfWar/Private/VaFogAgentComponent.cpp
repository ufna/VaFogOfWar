// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogAgentComponent.h"

#include "VaFogController.h"
#include "VaFogDefines.h"

#include "Components/BillboardComponent.h"

UVaFogAgentComponent::UVaFogAgentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	InteractionType = EVaFogAgentType::Dispel;

	bAgentEnabled = true;
	VisionRadius = 500;
	RadiusStrategy = EVaFogRadiusStrategy::Circle;
	HeightLevel = EVaFogHeightLevel::HL_3;

#if WITH_EDITORONLY_DATA
	bVisualizeComponent = true;
#endif
}

void UVaFogAgentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (bAgentEnabled)
	{
		UVaFogController::Get(this)->OnFogAgentAdded(this);
	}
}

void UVaFogAgentComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	if (bAgentEnabled && UVaFogController::Get(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		UVaFogController::Get(this)->OnFogAgentRemoved(this);
	}
}

#if WITH_EDITORONLY_DATA
void UVaFogAgentComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UpdateSpriteTexture();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UVaFogAgentComponent::OnRegister()
{
	Super::OnRegister();

	UpdateSpriteTexture();
}
#endif

void UVaFogAgentComponent::EnableAgent(bool bEnable)
{
	bAgentEnabled = bEnable;

	UpdateAgentRegistration();
}

void UVaFogAgentComponent::DisableAgent()
{
	bAgentEnabled = false;

	UpdateAgentRegistration();
}

bool UVaFogAgentComponent::IsAgentEnabled() const
{
	return bAgentEnabled;
}

void UVaFogAgentComponent::SetVisionRadius(int32 NewVisionRadius)
{
	VisionRadius = FMath::Max(NewVisionRadius, 0);
}

void UVaFogAgentComponent::SetHeightLevel(EVaFogHeightLevel NewHeightLevel)
{
	HeightLevel = NewHeightLevel;
}

void UVaFogAgentComponent::UpdateAgentRegistration()
{
	if (bAgentEnabled)
	{
		UVaFogController::Get(this)->OnFogAgentAdded(this);
	}
	else
	{
		UVaFogController::Get(this)->OnFogAgentRemoved(this);
	}
}

#if WITH_EDITORONLY_DATA
void UVaFogAgentComponent::UpdateSpriteTexture()
{
	if (SpriteComponent)
	{
		SpriteComponent->SpriteInfo.Category = TEXT("Misc");
		SpriteComponent->SpriteInfo.DisplayName = NSLOCTEXT("SpriteCategory", "Misc", "Misc");

		if (InteractionType == EVaFogAgentType::Obstacle)
		{
			SpriteComponent->SetSprite(LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EditorResources/S_Terrain.S_Terrain")));
		}
		else
		{
			SpriteComponent->SetSprite(LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EditorResources/S_Emitter.S_Emitter")));
		}
	}
}
#endif
