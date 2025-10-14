// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameplayAbility_ActiveSkill.h"
#include "AbilitySystemGlobals.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "AnimSequences/PaperZDAnimSequence.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/Animation/PaperZDAnimNotify_Damage.h"
#include "ProjectH/AbilitySystem/GameEffect/HDGE_Damage.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Util/UtilFunc_Sprite.h"
#include "ProjectH/Util/UtilFunc_Pooling.h"

UHDGameplayAbility_ActiveSkill::UHDGameplayAbility_ActiveSkill(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHDGameplayAbility_ActiveSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearCameraMode(true);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHDGameplayAbility_ActiveSkill::PlayFlipBookAnimation(FDynamicOnFlipbookComplete OnComplete)
{
	if (!AnimSequence)
		return;

	const TArray<UPaperZDAnimNotify_Base*>& Notifies = AnimSequence->GetAnimNotifies();

	if (Notifies.IsEmpty())
	{
		//Notify가 비활성화 되어있다면 바로 실행 시킨다.
		OnPlayEffect();
		OnExecute();
	}
	else
	{
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
	}

	Super::PlayFlipBookAnimation(OnComplete);
}

void UHDGameplayAbility_ActiveSkill::SetCameraMode(TSubclassOf<UModularCameraMode> CameraMode, bool UseFovOffset)
{
	UBattleSubsystem* BattleSubSystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
	if (!BattleSubSystem)
		return;

	FBattleStateParams* BattleStateParam = static_cast<FBattleStateParams*>(Params);
	if (!BattleStateParam)
		return;

	AActor* Actor;
	if (BattleStateParam->Objects.IsEmpty())
		Actor = GetAvatarActorFromActorInfo();
	else
		Actor = BattleStateParam->Objects[0];

	if (!Actor)
		return;

	BattleSubSystem->SetCameraMode(Actor, CameraMode, CurrentSpecHandle, UseFovOffset);
}

void UHDGameplayAbility_ActiveSkill::ClearCameraMode(bool UseFovOffest)
{
	UBattleSubsystem* BattleSubSystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
	if (!BattleSubSystem)
		return;

	AActor* Actor = GetAvatarActorFromActorInfo();
	if (!Actor)
		return;

	BattleSubSystem->ClearCameraMode(CurrentSpecHandle, UseFovOffest);
}

void UHDGameplayAbility_ActiveSkill::OnExecute()
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

	FSkillData* SkillData = const_cast<UHDAttributeSet*>(AttributeSet)->GetSkillData(BattleStateParam->SkillTag);

	if (!SkillData)
		return;

	for (AActor* TargetActor : BattleStateParam->Objects)
	{
		if (!IsValid(TargetActor))
			continue;

		UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(TargetActor);
		UHDAbilitySystemComponent* TargetASC = UtilFunc::GetASC(TargetActor);
		if (!BattleComp)
			continue;

		if (!BattleComp->CheckDead())
		{
			switch (SkillData->SkillValueType)
			{
			case ESkillValueType::Damage:
			{
				FDamageEffectContext* DamageContext = new FDamageEffectContext();
				DamageContext->AddInstigator(Actor, Actor);
				DamageContext->SkillTag = GetGameplayTag();
				TargetASC->ApplyGenericEffect(UHDGE_Damage::StaticClass(), DamageContext);
				break;
			}
			case ESkillValueType::Buff:
				TargetASC->RegisterBuff({ SkillData->Value }, Actor);
				break;
			case ESkillValueType::DeBuff:
				TargetASC->RegisterDebuff({ SkillData->Value }, Actor);
				break;
			default:
				break;
			}
		}

	}
}
