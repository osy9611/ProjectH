// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameplayAbility_BuffSkill.h"
#include "ProjectH/Animation/PaperZDAnimNotify_Buff.h"

UHDGameplayAbility_BuffSkill::UHDGameplayAbility_BuffSkill(const FObjectInitializer& ObjectInitializer)
{
}

void UHDGameplayAbility_BuffSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHDGameplayAbility_BuffSkill::PlayFlipBookAnimation(FDynamicOnFlipbookComplete OnComplete)
{
	if (!AnimSequence)
		return;

	const TArray<UPaperZDAnimNotify_Base*>& Notifies = AnimSequence->GetAnimNotifies();

	for (UPaperZDAnimNotify_Base* Notify : Notifies)
	{
		UPaperZDAnimNotify_Buff* DamageNotify = Cast<UPaperZDAnimNotify_Buff>(Notify);
		if (!DamageNotify)
			continue;

		DamageNotify->OnCallback = [this]()
			{
				OnPlayEffect();
			};
	}

	Super::PlayFlipBookAnimation(OnComplete);
}

void UHDGameplayAbility_BuffSkill::ApplyBuff()
{
}

void UHDGameplayAbility_BuffSkill::ExecuteGameEffect(UAbilitySystemComponent* OwnerASC, AActor* TargetActor)
{

}
