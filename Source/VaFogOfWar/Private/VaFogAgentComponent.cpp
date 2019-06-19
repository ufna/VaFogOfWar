// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogAgentComponent.h"

#include "VaFogController.h"
#include "VaFogDefines.h"

UVaFogAgentComponent::UVaFogAgentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
	bWantsInitializeComponent = true;
}

void UVaFogAgentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UVaFogController::Get(this)->OnFogAgentAdded(this);
}

void UVaFogAgentComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	UVaFogController::Get(this)->OnFogAgentRemoved(this);
}
