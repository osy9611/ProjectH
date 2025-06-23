// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/Battle/State/BattleStateManager.h"
#include "ProjectH/AbilitySystem/HDAbilitySystemComponent.h"
#include "BattleState.generated.h"

class UBattleWidget;
class UTurnManager;

struct FGameAbilityParam;

USTRUCT()
struct FBattleStateParams : public FGameAbilityParam
{
	GENERATED_BODY()
public:
	TArray<AActor*> Objects;

	virtual FGameAbilityParam* Clone() const override
	{
		return new FBattleStateParams(*this);
	}
};


UCLASS(Abstract)
class PROJECTH_API UBattleState : public UObject
{
	GENERATED_BODY()
public:
	UBattleState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Initailize();
	virtual void DoStart() {}
	virtual void DoEnd() {}
	virtual void DoExecute(const FBattleStateParams& Params){}	//즉시 State관련 실행을 하기 위해서 사용
	virtual void Update(float DeltaTime) {}
	virtual void ChangeState(EBattleState State);

	//전투의 경우 이벤트가 끝나면 시퀀스를 재생하는 일이 생긴다(예: 전투 복귀 등등)
	//이때 시퀀스가 끝나고 다음 로직을 실행을 StateManager에서 제어하기 위해서 추가함
	virtual void HandleEndSequence(TFunction<void()> Callback) {} 

protected:
	TFunction<void()> SequenceEndCallback;

	UPROPERTY()
	TWeakObjectPtr<UBattleWidget> BattleWidget;

	UPROPERTY()
	TWeakObjectPtr<UTurnManager> TurnManager;
};
