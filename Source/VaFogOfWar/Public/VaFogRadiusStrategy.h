// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** 
 * Abstract strategy pattern class that allows alternate radius calculations to be used. 
 * This allows for circular, square, diamond, or any custom formula for radius. 
 */
class FVaFogRadiusStrategy
{
public:
	FVaFogRadiusStrategy(){};
	virtual ~FVaFogRadiusStrategy() = 0;

	virtual bool IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y) { return false; };
};

class FVaFogRadiusStrategy_Circle : public FVaFogRadiusStrategy
{
public:
	virtual bool IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y) override;
};

class FVaFogRadiusStrategy_Square : public FVaFogRadiusStrategy
{
public:
	virtual bool IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y) override;
};

/** 
 * Radius treated as diameter here
 */
class FVaFogRadiusStrategy_SquareStepped : public FVaFogRadiusStrategy
{
public:
	virtual bool IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y) override;
};

/** Shared reference to the radius strategy */
using FVaFogRadiusStrategyRef = TSharedRef<FVaFogRadiusStrategy>;
