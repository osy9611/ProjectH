// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "UObject/NoExportTypes.h"
#include "BattleInput.generated.h"

class UHDInputConfig;

UCLASS()
class PROJECTH_API UBattleInput : public UObject
{
	GENERATED_BODY()
public:
	UBattleInput(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void OnInit();

private:
	UPROPERTY()
	TObjectPtr<UHDInputConfig> InputConfig;
};
