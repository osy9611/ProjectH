// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameplayAbility_DeBuffSkill.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Animation/PaperZDAnimNotify_Damage.h"

UHDGameplayAbility_DeBuffSkill::UHDGameplayAbility_DeBuffSkill(const FObjectInitializer& ObjectInitializer)
{
}

void UHDGameplayAbility_DeBuffSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHDGameplayAbility_DeBuffSkill::PlayFlipBookAnimation(FDynamicOnFlipbookComplete OnComplete)
{
	if (!AnimSequence)
		return;

	const TArray<UPaperZDAnimNotify_Base*>& Notifies = AnimSequence->GetAnimNotifies();

	for (UPaperZDAnimNotify_Base* Notify : Notifies)
	{
		UPaperZDAnimNotify_Damage* DamageNotify = Cast<UPaperZDAnimNotify_Damage>(Notify);
		if (!DamageNotify)
			continue;

		DamageNotify->OnCallback = [this]()
			{
				OnPlayEffect();
			};
	}

	Super::PlayFlipBookAnimation(OnComplete);
}

void UHDGameplayAbility_DeBuffSkill::ApplyDeBuff()
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

void UHDGameplayAbility_DeBuffSkill::ExecuteGameEffect(TArray<int32>& BuffIDs, UHDAbilitySystemComponent* OwnerASC, AActor* TargetActor)
{
	if (!OwnerASC || !TargetActor)
	{

		UE_LOG(HDLog, Warning, TEXT("[HDGameplayAbility_DeBuffSkill] Create GE Fail"));
		return;
	}

	AActor* SourceActor = GetAvatarActorFromActorInfo();
	UHDAbilitySystemComponent* TargetASC = UtilFunc::GetASC(TargetActor);
	TargetASC->RegisterDebuff(BuffIDs, SourceActor);
}