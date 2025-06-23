// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGE_Damage.h"
#include "AbilitySystemGlobals.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/AbilitySystem//AttributeSet/HDAttributeSet.h"

UHDGE_Damage::UHDGE_Damage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant; //한 프레임에 바로 실행되는 GE 타입
	Executions.Add(FGameplayEffectExecutionDefinition(UHDDamageExecutionCalculation::StaticClass()));
}

UHDDamageExecutionCalculation::UHDDamageExecutionCalculation()
{
}


//SkillMultiplier -> 스킬마다 고정된 데미지 배율 (예 : 1.2x, 2.0x 등)
//RandomVariance -> 0.95 ~ 1.05 사이의 랜덤 값 (데미지 흔들림)
//CritBonus -> 치명타 발생 시 ×1.25 ~ ×1.5 배 적용
//WeaknessBonus -> 약점을 찔렀을 때 보정 (×1.2 ~ ×1.5) + 실드 감소
void UHDDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

	//어떤 스킬을 사용했는지 들어있는 Context
	const FDamageEffectContext* DamageContext = static_cast<FDamageEffectContext*>(ContextHandle.Get());
	if (!DamageContext)
		return;

	//공격자 
	AActor* SourceActor = ContextHandle.GetOriginalInstigator();
	UAbilitySystemComponent* SourceASC = UtilFunc::GetASC(SourceActor);	
	if (!SourceASC)
		return;

	const UHDAttributeSet* SourceAttributeSet = SourceASC->GetSet<UHDAttributeSet>();
	if (!SourceAttributeSet)
		return;
	
	FSkillData* SourceSkillData = const_cast<UHDAttributeSet*>(SourceAttributeSet)->GetSkillData(DamageContext->SkillTag);
	if (!SourceSkillData)
		return;

	//대상자
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC) 
		return;

	const UHDAttributeSet* TargetAttr = TargetASC->GetSet<UHDAttributeSet>();
	if (!TargetAttr)
		return;

	//스탯 가져오기
	float ATK = (SourceSkillData->DamageType == EDamageType::Physical) ? SourceAttributeSet->GetPATK() : SourceAttributeSet->GetEATK();
	float DEF = (SourceSkillData->DamageType == EDamageType::Physical) ? TargetAttr->GetPDEF() : TargetAttr->GetEDEF();

	float DefenseFactor = DEF / (DEF + 100.0f);
	float MitigatedRate = 1.0f - DefenseFactor;

	float BaseDamage = ATK * MitigatedRate * SourceSkillData->AddDamageRate;

	//랜덤/크리티컬/약점 보정
	float Rand = FMath::FRandRange(0.95f, 1.05f);
	float CritBonus = SourceSkillData->UseCritical ? 1.5f : 1.0f;     // 예시: 치명타 적용 여부
	float WeakBonus = 1.0f; // 약점 보정은 이후 적용할 수 있음 (추후 Weakness 시스템 연계)

	float FinalDamage = BaseDamage * Rand * CritBonus * WeakBonus;

	//데미지 적용(HP 감소)
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UHDAttributeSet::GetHPAttribute(), EGameplayModOp::Additive, -FinalDamage));
}
