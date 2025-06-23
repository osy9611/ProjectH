// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGE_Damage.h"
#include "AbilitySystemGlobals.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/AbilitySystem//AttributeSet/HDAttributeSet.h"

UHDGE_Damage::UHDGE_Damage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant; //�� �����ӿ� �ٷ� ����Ǵ� GE Ÿ��
	Executions.Add(FGameplayEffectExecutionDefinition(UHDDamageExecutionCalculation::StaticClass()));
}

UHDDamageExecutionCalculation::UHDDamageExecutionCalculation()
{
}


//SkillMultiplier -> ��ų���� ������ ������ ���� (�� : 1.2x, 2.0x ��)
//RandomVariance -> 0.95 ~ 1.05 ������ ���� �� (������ ��鸲)
//CritBonus -> ġ��Ÿ �߻� �� ��1.25 ~ ��1.5 �� ����
//WeaknessBonus -> ������ ���� �� ���� (��1.2 ~ ��1.5) + �ǵ� ����
void UHDDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

	//� ��ų�� ����ߴ��� ����ִ� Context
	const FDamageEffectContext* DamageContext = static_cast<FDamageEffectContext*>(ContextHandle.Get());
	if (!DamageContext)
		return;

	//������ 
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

	//�����
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC) 
		return;

	const UHDAttributeSet* TargetAttr = TargetASC->GetSet<UHDAttributeSet>();
	if (!TargetAttr)
		return;

	//���� ��������
	float ATK = (SourceSkillData->DamageType == EDamageType::Physical) ? SourceAttributeSet->GetPATK() : SourceAttributeSet->GetEATK();
	float DEF = (SourceSkillData->DamageType == EDamageType::Physical) ? TargetAttr->GetPDEF() : TargetAttr->GetEDEF();

	float DefenseFactor = DEF / (DEF + 100.0f);
	float MitigatedRate = 1.0f - DefenseFactor;

	float BaseDamage = ATK * MitigatedRate * SourceSkillData->AddDamageRate;

	//����/ũ��Ƽ��/���� ����
	float Rand = FMath::FRandRange(0.95f, 1.05f);
	float CritBonus = SourceSkillData->UseCritical ? 1.5f : 1.0f;     // ����: ġ��Ÿ ���� ����
	float WeakBonus = 1.0f; // ���� ������ ���� ������ �� ���� (���� Weakness �ý��� ����)

	float FinalDamage = BaseDamage * Rand * CritBonus * WeakBonus;

	//������ ����(HP ����)
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UHDAttributeSet::GetHPAttribute(), EGameplayModOp::Additive, -FinalDamage));
}
