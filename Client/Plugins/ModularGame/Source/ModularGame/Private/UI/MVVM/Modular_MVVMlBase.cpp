// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularGame/Public/UI/MVVM/Modular_MVVMlBase.h"


void UModular_MVVMlBase::BeginDestroy()
{
	OnDestory();
	Super::BeginDestroy();
}

void UModular_MVVMlBase::Init(UAbilitySystemComponent* InASC)
{
	if (!InASC)
		return;
	ASC = InASC;
	RemoveASCValueChanges();
}

void UModular_MVVMlBase::OnDestory()
{
	RemoveASCValueChanges();
}

void UModular_MVVMlBase::RemoveASCValueChanges()
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