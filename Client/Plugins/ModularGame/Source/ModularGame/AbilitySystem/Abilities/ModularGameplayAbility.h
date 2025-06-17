// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ModularGameplayAbility.generated.h"

class UModularCameraMode;

UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	OnInputTirggered,
	WhileInputActive,
	OnSpawn
};

UCLASS()
class MODULARGAME_API UModularGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UModularGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable)
	virtual FGameplayTag GetGameplayTag() { return GetCurrentAbilitySpec()->DynamicAbilityTags.GetByIndex(0); }

	UFUNCTION(BlueprintCallable)
	virtual void SetCameraMode(TSubclassOf<UModularCameraMode> CameraMode, bool UseFovOffset = true);

	UFUNCTION(BlueprintCallable)
	virtual void ClearCameraMode(bool UseFovOffest = true);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityActivation")
	EAbilityActivationPolicy ActivationPolicy;

	TSubclassOf<UModularCameraMode> ActiveCameraMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimMontage")
	UAnimMontage* AnimMontage;
};
