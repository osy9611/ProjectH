// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "InputActionValue.h"
#include "HDInputConfig.h"
#include "EnhancedInputComponent.h"
#include "HDInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UHDInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	UHDInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	template <class UserClass, typename FuncType>
	void BindNativeAction(const UHDInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UHDInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);	
	/*template <class UserClass>
	void BindAbilityActions(const UHDInputConfig* InputConfig, UserClass* Object, FName PressedFunc, FName ReleasedFunc, TArray<uint32>& BindHandles);*/
};

template <class UserClass, typename FuncType>
void UHDInputComponent::BindNativeAction(const UHDInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);

	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
		BindAction(IA, TriggerEvent, Object, Func);
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UHDInputComponent::BindAbilityActions(const UHDInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	//AbilityAction에 대해서는 그냥 모든 InputAction에 다 바인딩 시킨다
	for (const FHDInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());

			if (ReleasedFunc)
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
		}
	}
}
//
//template<class UserClass>
//void UHDInputComponent::BindAbilityActions(const UHDInputConfig* InputConfig, UserClass* Object, FName PressedFunc, FName ReleasedFunc, TArray<uint32>& BindHandles)
//{
//	check(InputConfig);
//
//	//AbilityAction에 대해서는 그냥 모든 InputAction에 다 바인딩 시킨다
//	for (const FHDInputAction& Action : InputConfig->AbilityInputActions)
//	{
//		if (Action.InputAction && Action.InputTag.IsValid())
//		{
//			if (PressedFunc.IsNone())
//				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
//
//			if (ReleasedFunc.IsNone())
//				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
//		}
//	}
//}
