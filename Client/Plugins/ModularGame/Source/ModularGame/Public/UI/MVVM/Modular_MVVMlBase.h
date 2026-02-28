// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "AbilitySystemComponent.h"
#include "ModularGame/Public/Log/ModularLogChannel.h"
#include "Modular_MVVMlBase.generated.h"

class UAbilitySystemComponent;

UCLASS()
class MODULARGAME_API UModular_MVVMlBase : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	virtual void BeginDestroy() override;
public:
	//ASC
	virtual void Init(UAbilitySystemComponent* InASC);

	virtual void OnDestory();
	virtual void RemoveASCValueChanges();

	template<typename T>
	void BindASCValueChange(FGameplayAttribute Attribute, T* Object, void(T::* Func)(const FOnAttributeChangeData&));

	template<typename T>
	void BindField(const UE::FieldNotification::FFieldId FieldId, T* Object, void (T::* Func)(UObject*, UE::FieldNotification::FFieldId));

protected:
	TWeakObjectPtr<UAbilitySystemComponent> ASC;

private:
	TMap<FGameplayAttribute, FDelegateHandle> ChangeValueHandles;
	TMap<const UE::FieldNotification::FFieldId, FDelegateHandle> VMFiledHandles;
};

template<typename T>
void UModular_MVVMlBase::BindASCValueChange(FGameplayAttribute Attribute, T* Object, void(T::* Func)(const FOnAttributeChangeData&))
{
	if (!ASC.IsValid())
	{
		//UE_LOG(Modular, Log, TEXT("[Modular_MVVMlBase] ASC is nullptr"));
		return;
	}

	if (FDelegateHandle* ExistingHandle = ChangeValueHandles.Find(Attribute))
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Attribute)
			.Remove(*ExistingHandle);

		ChangeValueHandles.Remove(Attribute);
	}

	FDelegateHandle NewHandle = ASC->GetGameplayAttributeValueChangeDelegate(Attribute)
		.AddUObject(Object, Func);
	ChangeValueHandles.Add(Attribute, NewHandle);

}

template<typename T>
void UModular_MVVMlBase::BindField(const UE::FieldNotification::FFieldId FieldId, T* Object, void(T::* Func)(UObject*, UE::FieldNotification::FFieldId))
{
	if (FDelegateHandle* ExistingHandle = VMFiledHandles.Find(FieldId))
	{
		RemoveFieldValueChangedDelegate(FieldId, *ExistingHandle);
		VMFiledHandles.Remove(FieldId);
	}

	FDelegateHandle Handle = AddFieldValueChangedDelegate(
		FieldId,
		FFieldValueChangedDelegate::CreateUObject(Object, Func));

	VMFiledHandles.Add(FieldId, Handle);
}
