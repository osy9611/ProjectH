// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "ProjectH/AbilitySystem/Abilities/HDGameplayAbility.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "HDGameplayAbility_BuffSkill.generated.h"

class UHDAbilitySystemComponent;

UCLASS()
class PROJECTH_API UHDGameplayAbility_BuffSkill : public UHDGameplayAbility
{
	GENERATED_BODY()
public:
	UHDGameplayAbility_BuffSkill(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void PlayFlipBookAnimation(FDynamicOnFlipbookComplete OnComplete);

	UFUNCTION(BlueprintCallable)
	void ApplyBuff();

private:
	void ExecuteGameEffect(TArray<int32>& BuffIDs, UHDAbilitySystemComponent* OwnerASC, AActor* TargetActor);
};
