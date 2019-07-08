// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogController.h"

#include "VaFogAgentComponent.h"
#include "VaFogBoundsVolume.h"
#include "VaFogDefines.h"
#include "VaFogLayerComponent.h"
#include "VaFogOfWar.h"

UVaFogController::UVaFogController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UVaFogController* UVaFogController::Get(UObject* WorldContextObject, EGetWorldErrorMode ErrorMode)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, ErrorMode))
	{
		return FVaFogOfWarModule::Get().GetFogController(World);
	}

	return nullptr;
}

void UVaFogController::OnFogBoundsAdded(AVaFogBoundsVolume* InFogVolume)
{
	FogVolume = InFogVolume;

	UE_LOG(LogVaFog, Log, TEXT("[%s] Added: %s"), *VA_FUNC_LINE, *InFogVolume->GetName());
}

void UVaFogController::OnFogBoundsRemoved(AVaFogBoundsVolume* InFogVolume)
{
	if (FogVolume == InFogVolume)
	{
		FogVolume = nullptr;

		UE_LOG(LogVaFog, Log, TEXT("[%s] Removed: %s"), *VA_FUNC_LINE, *InFogVolume->GetName());
	}
	else
	{
		UE_LOG(LogVaFog, Error, TEXT("[%s] Current fog volume is different from we're trying to remove: Current: %s, Removing: %s"),
			*VA_FUNC_LINE, (FogVolume != nullptr) ? *FogVolume->GetName() : TEXT("invalid"), *InFogVolume->GetName());
	}
}

void UVaFogController::OnFogLayerAdded(UVaFogLayerComponent* InFogLayer)
{
	FogLayers.AddUnique(InFogLayer);

	UE_LOG(LogVaFog, Log, TEXT("[%s] Added: %s %d"), *VA_FUNC_LINE, *InFogLayer->GetName(), (int32)InFogLayer->LayerChannel);
}

void UVaFogController::OnFogLayerRemoved(UVaFogLayerComponent* InFogLayer)
{
	int32 RemovedLayersNum = FogLayers.Remove(InFogLayer);
	if (RemovedLayersNum == 0)
	{
		UE_LOG(LogVaFog, Error, TEXT("[%s] No cached data found for: %s"), *VA_FUNC_LINE, *InFogLayer->GetName());
	}

	UE_LOG(LogVaFog, Log, TEXT("[%s] Removed: %s"), *VA_FUNC_LINE, *InFogLayer->GetName());
}

void UVaFogController::OnFogAgentAdded(UVaFogAgentComponent* InFogAgent)
{
	if (auto TargetFogLayer = GetFogLayer(InFogAgent->TargetChannel))
	{
		TargetFogLayer->AddFogAgent(InFogAgent);

		UE_LOG(LogVaFog, Log, TEXT("[%s] Added: %s"), *VA_FUNC_LINE, *InFogAgent->GetName());
	}
	else
	{
		UE_LOG(LogVaFog, Warning, TEXT("[%s] No suitable fog layer found for: %s (%d)"), *VA_FUNC_LINE, *InFogAgent->GetName(), (int32)InFogAgent->TargetChannel);
	}
}

void UVaFogController::OnFogAgentRemoved(UVaFogAgentComponent* InFogAgent)
{
	if (auto TargetFogLayer = GetFogLayer(InFogAgent->TargetChannel))
	{
		TargetFogLayer->RemoveFogAgent(InFogAgent);

		UE_LOG(LogVaFog, Log, TEXT("[%s] Removed: %s"), *VA_FUNC_LINE, *InFogAgent->GetName());
	}
}

AVaFogBoundsVolume* UVaFogController::GetFogVolume() const
{
	return (FogVolume.IsValid()) ? FogVolume.Get() : nullptr;
}

UVaFogLayerComponent* UVaFogController::GetFogLayer(EVaFogLayerChannel LayerChannel) const
{
	auto FogLayerPtr = FogLayers.FindByPredicate([LayerChannel](const TWeakObjectPtr<UVaFogLayerComponent> InLayer) {
		return (InLayer.IsValid()) ? (InLayer.Get()->LayerChannel == LayerChannel) : false;
	});

	return (FogLayerPtr && (*FogLayerPtr).IsValid()) ? ((*FogLayerPtr).Get()) : nullptr;
}
