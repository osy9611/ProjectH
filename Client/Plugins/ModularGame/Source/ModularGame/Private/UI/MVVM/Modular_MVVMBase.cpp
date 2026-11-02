// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularGame/Public/UI/MVVM/Modular_MVVMBase.h"


void UModular_MVVMBase::BeginDestroy()
{
	OnDestory();
	Super::BeginDestroy();
}

void UModular_MVVMBase::Init(UAbilitySystemComponent* InASC)
{
	if (!InASC)
		return;
	ASC = InASC;
	RemoveASCValueChanges();
}

void UModular_MVVMBase::OnDestory()
{
	RemoveASCValueChanges();
}

void UModular_MVVMBase::RemoveASCValueChanges()
{
	if (!ASC.IsValid())
		return;

	if (ChangeValueHandles.IsEmpty())
		return;

	for (auto& Handle : ChangeValueHandles)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Handle.Key).Remove(Handle.Value);
	}

	ChangeValueHandles.Empty();
}