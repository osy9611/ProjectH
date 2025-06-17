// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/AbilitySystem/Abilities/ModularGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "HDGameplayAbility.generated.h"

class UModularCameraMode;

UCLASS()
class PROJECTH_API UHDGameplayAbility : public UModularGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void SetCameraMode(TSubclassOf<UModularCameraMode> CameraMode, bool UseFovOffset = true) override;
	virtual void ClearCameraMode(bool UseFovOffest = true) override;

};
