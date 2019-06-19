// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogLayerComponent.h"

#include "VaFogAgentComponent.h"
#include "VaFogBoundsVolume.h"
#include "VaFogController.h"
#include "VaFogDefines.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Materials/Material.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

UVaFogLayerComponent::UVaFogLayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("MaterialInstanceConstant'/VaFogOfWar/Materials/MI_Blur.MI_Blur'"));
	BlurMaterial = Material.Object;

	bAutoActivate = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_DuringPhysics;
}

void UVaFogLayerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UVaFogController::Get(this)->OnFogLayerAdded(this);
}

void UVaFogLayerComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	if (UVaFogController::Get(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		UVaFogController::Get(this)->OnFogLayerRemoved(this);
	}
}

void UVaFogLayerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto FogVolume = UVaFogController::Get(this)->GetFogVolume();

	for (auto FogAgent : FogAgents)
	{
		FIntPoint AgentLocation = FogVolume->TransformWorldToLayer(FogAgent->GetOwner()->GetActorLocation());

		DrawDebugSphere(GetWorld(), FogAgent->GetOwner()->GetActorLocation(), FogAgent->VisionRadius, 32, FColor::Red, false, 0.0f);
	}
}

void UVaFogLayerComponent::AddFogAgent(UVaFogAgentComponent* FogAgent)
{
	FogAgents.AddUnique(FogAgent);
}

void UVaFogLayerComponent::RemoveFogAgent(UVaFogAgentComponent* InFogAgent)
{
	int32 NumRemoved = FogAgents.Remove(InFogAgent);
	if (NumRemoved == 0)
	{
		UE_LOG(LogVaFog, Error, TEXT("[%s] No cached data found for: %s"), *VA_FUNC_LINE, *InFogAgent->GetName());
	}
}
