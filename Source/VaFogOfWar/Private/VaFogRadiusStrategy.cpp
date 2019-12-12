// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogRadiusStrategy.h"

FVaFogRadiusStrategy::~FVaFogRadiusStrategy() = default;

bool FVaFogRadiusStrategy_Circle::IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y)
{
	return ((X - CenterX) * (X - CenterX) + (Y - CenterY) * (Y - CenterY)) < FMath::Square(Radius);
}

bool FVaFogRadiusStrategy_Square::IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y)
{
	return (FMath::Abs(X - CenterX) < Radius &&
			FMath::Abs(Y - CenterY) < Radius);
}

bool FVaFogRadiusStrategy_SquareStepped::IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y)
{
	float HalfRadius = static_cast<float>(Radius) / 2.f;
	int32 HalfRadiusX = (X > CenterX) ? FMath::CeilToInt(HalfRadius) : FMath::FloorToInt(HalfRadius);
	int32 HalfRadiusY = (Y > CenterY) ? FMath::CeilToInt(HalfRadius) : FMath::FloorToInt(HalfRadius);

	return (FMath::Abs(X - CenterX) < HalfRadiusX &&
			FMath::Abs(Y - CenterY) < HalfRadiusY);
}
