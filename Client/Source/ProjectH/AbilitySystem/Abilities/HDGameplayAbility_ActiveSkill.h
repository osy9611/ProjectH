// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "ProjectH/AbilitySystem/Abilities/HDGameplayAbility.h"
#include "HDGameplayAbility_ActiveSkill.generated.h"

UCLASS()
class PROJECTH_API UHDGameplayAbility_ActiveSkill : public UHDGameplayAbility
{
	GENERATED_BODY()

public:
	UHDGameplayAbility_ActiveSkill(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void PlayFlipBookAnimation(FDynamicOnFlipbookComplete OnComplete);
private:
	void ApplyDamage();
	void ExecuteGameEffect(UAbilitySystemComponent* OwnerASC,AActor* TargetActor);
};
