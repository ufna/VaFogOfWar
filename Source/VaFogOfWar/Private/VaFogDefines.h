// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogCategory.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"

DECLARE_STATS_GROUP(TEXT("VA Fog of War"), STATGROUP_VaFog, STATCAT_Advanced);

DECLARE_LOG_CATEGORY_EXTERN(LogVaFog, Log, All);

#define VA_FUNC (FString(__FUNCTION__))              // Current Class Name + Function Name where this is called
#define VA_LINE (FString::FromInt(__LINE__))         // Current Line Number in the code where this is called
#define VA_FUNC_LINE (VA_FUNC + "(" + VA_LINE + ")") // Current Class and Line Number where this is called!
