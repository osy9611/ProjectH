// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ModularAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class MODULARGAME_API UModularAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UModularAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	/*
	* GAS���� �ɷ��� �����ϱ� ���� �Է��� �����Ǿ����� Ȯ���� �� ���
	*/
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	virtual void ProcessAbility(const FGameplayTag& Tag);

	virtual UGameplayAbility* GetAbility(const FGameplayTag& Tag);

	virtual void AbilityInputTagPressed(const FGameplayTag& InputTag);
	virtual void AbilityInputTagReleased(const FGameplayTag& InputTag);
	virtual void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

protected:
	//Ability Input�� ó���� PendingQueue
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
};
