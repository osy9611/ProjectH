// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ModularGame/Public/AbilitySystem/ModularAbilitySystemComponent.h"
#include "ProjectH/AbilitySystem/HDGameAbilityParam.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "HDAbilitySystemComponent.generated.h"

struct FDeBuffHandle;
struct FDamageEffectContext;

UCLASS()
class PROJECTH_API UHDAbilitySystemComponent : public UModularAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UHDAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void ProcessAbilityAndParam(const FGameplayTag& Tag, const FGameAbilityParam& Params);
	void StoreParamsForAbility(FGameplayAbilitySpecHandle Handle, const FGameAbilityParam& Params);
	FGameAbilityParam* ConsumeParams(FGameplayAbilitySpecHandle Handle);

	//턴제의 경우에는 턴 기반으로 버프를 관리해야하기 떄문에 카운트를 ASC에서 별도로 관리를 해야한다.
	void RegisterBuff(TArray<int32> BuffIDs,AActor* Source);
	void RegisterDebuff(TArray<int32> BuffIDs, AActor* Source);

	FActiveGameplayEffectHandle ApplyGenericEffect(TSubclassOf<UGameplayEffect> GEClass, FGameplayEffectContext* Context);

	void UpdateGE();
private:
	void UpdateBuff();
	void UpdateDeBuff();

	void AddBuffHandle(FActiveGameplayEffectHandle& Handle, int32 TurnCount);
	void ResetBuffHandle(FActiveGameplayEffectHandle& Handle, int32 TurnCount);

	void AddDeBuffHandle(FActiveGameplayEffectHandle& Handle, int32 TurnCount);
	void ResetDeBuffHandle(FActiveGameplayEffectHandle& Handle, int32 TurnCount);

private:
	TMap<FGameplayAbilitySpecHandle, FGameAbilityParam*> PendingParams;

	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, int32> BuffTurns;

	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, int32> DeBuffTurns;
};
