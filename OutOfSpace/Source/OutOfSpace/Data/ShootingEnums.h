#pragma once

#include "ShootingEnums.generated.h"

UENUM(BlueprintType)
enum class EProjectileType : uint8 
{
	Normal,
	Homing
};