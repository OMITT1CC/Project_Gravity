#pragma once
#include "CoreMinimal.h"
#include "Global.generated.h"

float const PlaneGravityMagnitude = 981.0;
float const ParticleMassMultiplier = 1.0f;
double const GravitationalConstant = 6.67430E-11;

UENUM(BlueprintType)
enum class ECustomGravityState : uint8
{
	Plane	UMETA(DisplayName = "Plane"),
	Sphere	UMETA(DisplayName = "Sphere"),
	Default	UMETA(DisplayName = "Default")
};