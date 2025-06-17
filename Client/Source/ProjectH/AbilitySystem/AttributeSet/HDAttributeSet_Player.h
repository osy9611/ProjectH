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

	void OnInit(FString InitTableID) override;
	void OnUpdateStatus() override;

	float GetOriginHP() { return OriginHP; }
	float GetOriginMP() { return OriginMP; }

private:
	float OriginHP;
	float OriginMP;
};
