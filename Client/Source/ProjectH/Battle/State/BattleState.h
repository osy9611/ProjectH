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
	virtual void DoExecute(const FBattleStateParams& Params){}	//��� State���� ������ �ϱ� ���ؼ� ���
	virtual void Update(float DeltaTime) {}
	virtual void ChangeState(EBattleState State);

	//������ ��� �̺�Ʈ�� ������ �������� ����ϴ� ���� �����(��: ���� ���� ���)
	//�̶� �������� ������ ���� ������ ������ StateManager���� �����ϱ� ���ؼ� �߰���
	virtual void HandleEndSequence(TFunction<void()> Callback) {} 

protected:
	TFunction<void()> SequenceEndCallback;

	UPROPERTY()
	TWeakObjectPtr<UBattleWidget> BattleWidget;

	UPROPERTY()
	TWeakObjectPtr<UTurnManager> TurnManager;
};
