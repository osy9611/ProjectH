// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameplayAbility_ActiveSkill.h"
#include "AnimSequences/PaperZDAnimSequence.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Animation/PaperZDAnimNotify_Damage.h"
UHDGameplayAbility_ActiveSkill::UHDGameplayAbility_ActiveSkill(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHDGameplayAbility_ActiveSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHDGameplayAbility_ActiveSkill::PlayFlipBookAnimation(FDynamicOnFlipbookComplete OnComplete)
{
	if (!AnimSequence)
		return;

	const TArray<UPaperZDAnimNotify_Base*>& Notifies = AnimSequence->GetAnimNotifies();

	for (UPaperZDAnimNotify_Base* Notify : Notifies)
	{
		UPaperZDAnimNotify_Damage* DamageNotify = Cast<UPaperZDAnimNotify_Damage>(Notify);
		if (!DamageNotify)
			continue;

		DamageNotify->OnCallback = []()
			{
				UE_LOG(HDLog, Log, TEXT("[HDGameplayAbility_ActiveSkill] Notify Call"));
			};
	}

	Super::PlayFlipBookAnimation(OnComplete);
}
