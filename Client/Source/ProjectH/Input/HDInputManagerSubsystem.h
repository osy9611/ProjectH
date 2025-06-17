// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ProjectH/Input/HDMappableConfigPair.h"
#include "ProjectH/Input/Container/HDInputContainer.h"
#include "ProjectH/Input/HDInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HDInputManagerSubsystem.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UCommonLocalPlayer;
class UInputMappingContext;
class UHDInputConfig;

struct FInputActionValue;
struct FHDMappableConfigPair;


UCLASS()
class PROJECTH_API UHDInputManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UHDInputManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer);
	virtual void NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer);
	virtual void NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer);

	//HeroComponent나 직접적으로 등록을 할때는 해당 코드를 쓴다.
	void RegisterHDMappableConfigPair(const FHDMappableConfigPair& Pair);
	void RegisterHDMappableConfigPairs(const TArray<FHDMappableConfigPair>& Pairs);

	void LoadInputContext(EInputMappingType Type, bool AutoApply);
	void AddInputContext(UInputMappingContext* Context);
	void ApplyContext(UInputMappingContext* Context, int32 Priority);

	void RemoveInputContext(UInputMappingContext* Context);
	void RemoveAllInputContexts();

	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UHDInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	template <class UserClass, typename FuncType>
	void BindNativeAction(const UHDInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);


private:
	UEnhancedInputLocalPlayerSubsystem* GetEnhanceSubsystem();

	UPROPERTY()
	TArray<TObjectPtr<UInputMappingContext>> MappingContexts;

	UPROPERTY()
	TArray<TObjectPtr<UInputMappingContext>> ActiveMappings;

};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UHDInputManagerSubsystem::BindAbilityActions(const UHDInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
		return;

	if (UHDInputComponent* InputComp = Cast<UHDInputComponent>(PlayerController->InputComponent))
	{
		TArray<uint32> BindHandles;
		InputComp->BindAbilityActions(InputConfig, Object, PressedFunc, ReleasedFunc, BindHandles);
	}
}
PRAGMA_DISABLE_OPTIMIZATION
template<class UserClass, typename FuncType>
void UHDInputManagerSubsystem::BindNativeAction(const UHDInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
		return;

	if (UHDInputComponent* InputComp = Cast<UHDInputComponent>(PlayerController->InputComponent))
	{
		InputComp->BindNativeAction(InputConfig, InputTag, TriggerEvent, Object, Func, bLogIfNotFound);
	}
}
PRAGMA_ENABLE_OPTIMIZATION