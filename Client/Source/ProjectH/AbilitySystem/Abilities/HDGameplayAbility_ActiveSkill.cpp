// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameplayAbility_ActiveSkill.h"
#include "AbilitySystemGlobals.h"
#include "AnimSequences/PaperZDAnimSequence.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "ProjectH/Animation/PaperZDAnimNotify_Damage.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/AbilitySystem/GameEffect/HDGE_Damage.h"
#include "ProjectH/Util/UtilFunc.h"

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

		DamageNotify->OnCallback = [this]()
			{
				UE_LOG(HDLog, Log, TEXT("[HDGameplayAbility_ActiveSkill] Notify Call"));
				ApplyDamage();
			};
	}

	Super::PlayFlipBookAnimation(OnComplete);
}

void UHDGameplayAbility_ActiveSkill::ApplyDamage()
{
	FBattleStateParams* BattleStateParam = static_cast<FBattleStateParams*>(Params);
	if (!BattleStateParam)
		return;

	FGameplayTag Tag = GetGameplayTag();

	AActor* Actor = GetAvatarActorFromActorInfo();
	if (!Actor)
		return;

	UAbilitySystemComponent* ASC = UtilFunc::GetASC(Actor);
	if (!ASC)
		return;

	const UHDAttributeSet* AttributeSet = ASC->GetSet<UHDAttributeSet>();
	if (!AttributeSet)
		return;

	for (AActor* TargetActor : BattleStateParam->Objects)
	{
		if (!IsValid(TargetActor))
			continue;

		UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(TargetActor);
		if (!BattleComp)
			continue;

		if (!BattleComp->CheckDead())
			ExecuteGameEffect(ASC, TargetActor);
	}

}

void UHDGameplayAbility_ActiveSkill::ExecuteGameEffect(UAbilitySystemComponent* OwnerASC,AActor* TargetActor)
{
	if (!OwnerASC || !TargetActor)
	{
		UE_LOG(HDLog, Warning, TEXT("[HDGameplayAbility_ActiveSkill] Create GE Fail"));
		return;
	}

	AActor* SourceActor = GetAvatarActorFromActorInfo();
	UHDAbilitySystemComponent* TargetASC = UtilFunc::GetASC(TargetActor);

	FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();

	FDamageEffectContext* DamageContext = new FDamageEffectContext();
	DamageContext->AddInstigator(SourceActor, SourceActor);
	DamageContext->SkillTag = GetGameplayTag();

	FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle(DamageContext);

	//Create GE Spec
	TSubclassOf<UGameplayEffect> GEClass = UHDGE_Damage::StaticClass();
	FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(GEClass, 1.0f, ContextHandle);

	if (SpecHandle.IsValid())
	{
		OwnerASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}
