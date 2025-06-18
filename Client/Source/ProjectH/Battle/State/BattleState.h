// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/Battle/State/BattleStateManager.h"
#include "BattleState.generated.h"

class UBattleWidget;
class UTurnManager;
UCLASS(Abstract)
class PROJECTH_API UBattleState : public UObject
{
	GENERATED_BODY()
public:
	UBattleState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Initailize();
	virtual void DoStart() {}
	virtual void DoEnd() {}
	virtual void DoExecute(){}	//��� State���� ������ �ϱ� ���ؼ� ���
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
