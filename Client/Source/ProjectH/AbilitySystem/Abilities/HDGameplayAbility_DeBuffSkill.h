// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/AbilitySystem/Abilities/HDGameplayAbility.h"
#include "HDGameplayAbility_DeBuffSkill.generated.h"

class UHDAbilitySystemComponent;

UCLASS()
class PROJECTH_API UHDGameplayAbility_DeBuffSkill : public UHDGameplayAbility
{
	GENERATED_BODY()
public:
	UHDGameplayAbility_DeBuffSkill(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void PlayFlipBookAnimation(FDynamicOnFlipbookComplete OnComplete);
	
	UFUNCTION(BlueprintCallable)
	void ApplyDeBuff();

private:
	void ExecuteGameEffect(TArray<int32>& BuffIDs, UHDAbilitySystemComponent* OwnerASC, AActor* TargetActor);
};
