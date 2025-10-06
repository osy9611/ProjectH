// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameplayAbility_BuffSkill.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Animation/PaperZDAnimNotify_Buff.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/AbilitySystem/GameEffect/HDGE_Buff.h"
#include "ProjectH/AbilitySystem//AttributeSet/HDAttributeSet.h"

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
	FBattleStateParams* BattleStateParam = static_cast<FBattleStateParams*>(Params);
	if (!BattleStateParam)
		return;

	AActor* Actor = GetAvatarActorFromActorInfo();
	if (!Actor)
		return;

	UHDAbilitySystemComponent* ASC = UtilFunc::GetASC(Actor);
	if (!ASC)
		return;

	for (AActor* TargetActor : BattleStateParam->Objects)
	{
		if (!IsValid(TargetActor))
			continue;

		UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(TargetActor);
		if (!BattleComp)
			continue;

		if (!BattleComp->CheckDead())
			ExecuteGameEffect(BattleStateParam->BuffIDs, ASC, TargetActor);
	}
}

void UHDGameplayAbility_BuffSkill::ExecuteGameEffect(TArray<int32>& BuffIDs, UHDAbilitySystemComponent* OwnerASC, AActor* TargetActor)
{
	if (!OwnerASC || !TargetActor)
	{
		UE_LOG(HDLog, Warning, TEXT("[HDGameplayAbility_BuffSkill] Create GE Fail"));
		return;
	}

	AActor* SourceActor = GetAvatarActorFromActorInfo();
	UHDAbilitySystemComponent* TargetASC = UtilFunc::GetASC(TargetActor);
	TargetASC->RegisterBuff(BuffIDs, SourceActor);
}
