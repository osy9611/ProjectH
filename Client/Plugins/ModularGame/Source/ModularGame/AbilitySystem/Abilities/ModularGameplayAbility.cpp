// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularGameplayAbility.h"
#include "ModularGame/Camera/ModularCameraMode.h"

UModularGameplayAbility::UModularGameplayAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UModularGameplayAbility::SetCameraMode(TSubclassOf<UModularCameraMode> CameraMode, bool UseFovOffset)
{

}

void UModularGameplayAbility::ClearCameraMode(bool UseFovOffest)
{
}

void UModularGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UModularGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}