// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGEComp_Damage_Base.h"
#include "AbilitySystemComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/AbilitySystem/GameEffect/HDGE_Damage.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"

void UHDGEComp_Damage_Base::OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	Super::OnGameplayEffectExecuted(ActiveGEContainer, GESpec, PredictionKey);
	UE_LOG(HDLog, Log, TEXT("[HDGEComp_Damage_Base] OnGameplayEffectExecuted"));

	const FGameplayEffectContextHandle& ContextHandle = GESpec.GetContext();
	const FDamageEffectContext* DamageContext = static_cast<const FDamageEffectContext*>(ContextHandle.Get());
	if (!DamageContext)
		return;
	//공격자
	AActor* SourceActor = ContextHandle.GetOriginalInstigator();
	UAbilitySystemComponent* SourceASC = UtilFunc::GetASC(SourceActor);
	if (!SourceActor)
		return;

	const UHDAttributeSet* SourceAttributeSet = SourceASC->GetSet<UHDAttributeSet>();
	if (!SourceAttributeSet)
		return;

	FSkillData* SourceSkillData = const_cast<UHDAttributeSet*>(SourceAttributeSet)->GetSkillData(DamageContext->SkillTag);
	if (!SourceSkillData)
		return;

	//대상자
	UAbilitySystemComponent* TargetASC = ActiveGEContainer.Owner;
	if (!TargetASC)
		return;

	const UHDAttributeSet* TargetAttr = TargetASC->GetSet<UHDAttributeSet>();
	if (!TargetAttr)
		return;

	//스탯 가져오기
	float ATK = (SourceSkillData->DamageType == EDamageType::Physical) ? SourceAttributeSet->GetPATK() : SourceAttributeSet->GetEATK();
	float ATK_BuffAmount = (SourceSkillData->DamageType == EDamageType::Physical) ? SourceAttributeSet->GetG1_PATK() : SourceAttributeSet->GetG1_EATK();

	//총 공격력 = 공격력 + (공격력 * (공격력 버프 * 0.01f))
	ATK = ATK + (ATK * (ATK_BuffAmount * 0.01f));

	float DEF = (SourceSkillData->DamageType == EDamageType::Physical) ? TargetAttr->GetPDEF() : TargetAttr->GetEDEF();
	float DEF_BuffAmount = (SourceSkillData->DamageType == EDamageType::Physical) ? TargetAttr->GetG1_PDEF() : TargetAttr->GetG1_EDEF();

	//총 방어력 = 방어력 + (방어력 * (방어력 버프 * 0.01f))
	DEF = DEF + (DEF * (DEF_BuffAmount * 0.01f));

	float DefenseFactor = DEF / (DEF + 100.0f);
	float MitigatedRate = 1.0f - DefenseFactor;

	float BaseDamage = ATK * MitigatedRate * SourceSkillData->AddDamageRate;

	//랜덤/크리티컬/약점 보정
	float Rand = FMath::FRandRange(0.95f, 1.05f);
	float CritBonus = SourceSkillData->UseCritical ? 1.5f : 1.0f;     // 예시: 치명타 적용 여부
	float WeakBonus = 1.0f; // 약점 보정은 이후 적용할 수 있음 (추후 Weakness 시스템 연계)

	float FinalDamage = BaseDamage * Rand * CritBonus * WeakBonus;

	TargetASC->ApplyModToAttributeUnsafe(UHDAttributeSet::GetHPAttribute(), EGameplayModOp::Additive, -FinalDamage);
}
