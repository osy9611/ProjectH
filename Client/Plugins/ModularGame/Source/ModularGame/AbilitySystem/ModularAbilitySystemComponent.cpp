// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularAbilitySystemComponent.h"
#include "ModularGame/AbilitySystem/Abilities/ModularGameplayAbility.h"
UModularAbilitySystemComponent::UModularAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UModularAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnActor = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}

void UModularAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
	
	/*
	* InvokeReplicatedEvent
	* GAS에서 특정 이벤트가 발생했을 때 이벤트를 복제하여 처리하는 역할을 담당한다.
	* 클라이언트와 서버 간의 능력 이벤트 동기화를 위해 사용된다.
	* 멀티 플레이가 아니더라도 재사용
	*/
	if (Spec.IsActive())
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
}

void UModularAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
}

void UModularAbilitySystemComponent::ProcessAbility(const FGameplayTag& Tag)
{
	if (Tag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && !AbilitySpec.IsActive() && (AbilitySpec.DynamicAbilityTags.HasTagExact(Tag)))
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UModularAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{

				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UModularAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UModularAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	TArray<FGameplayAbilitySpecHandle> ExecuteHandles;

	for (const FGameplayAbilitySpecHandle& Handle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UModularGameplayAbility* AbilityCDO = CastChecked<UModularGameplayAbility>(AbilitySpec->Ability);

				if (AbilityCDO->ActivationPolicy == EAbilityActivationPolicy::WhileInputActive)
					ExecuteHandles.AddUnique(AbilitySpec->Handle);
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& Handle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;
				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UModularGameplayAbility* AbilityCDO = CastChecked<UModularGameplayAbility>(AbilitySpec->Ability);

					if (AbilityCDO->ActivationPolicy == EAbilityActivationPolicy::OnInputTirggered)
						ExecuteHandles.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	//Handle 등록시작
	for (const FGameplayAbilitySpecHandle& Handle : ExecuteHandles)
	{
		TryActivateAbility(Handle);
	}

	//이번 프레임에 Release되었다면, 관련 GameplayAbility 처리
	for (const FGameplayAbilitySpecHandle& Handle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->Ability.GetName();
				AbilitySpec->InputPressed = false;
				if (AbilitySpec->IsActive())
					AbilitySpecInputReleased(*AbilitySpec);
			}
		}
	}

	//InputHeldSpecHandles는 InputReleaseSpecHandles가 추가될때 제거된다
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}
