// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "HDAttributeSet_Player.generated.h"

UCLASS()
class PROJECTH_API UHDAttributeSet_Player : public UHDAttributeSet
{
	GENERATED_BODY()
public:
	UHDAttributeSet_Player(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnInit(FString InitTableID) override;
	virtual void OnUpdateStatus() override;

	float GetOriginHP() { return OriginHP; }
	float GetOriginSP() { return OriginSP; }



private:
	float OriginHP;
	float OriginSP;
};
