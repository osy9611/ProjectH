// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "BattleState_Ready.generated.h"

class ULevelSequence;
UCLASS()
class PROJECTH_API UBattleState_Ready : public UBattleState
{
	GENERATED_BODY()
public:
	virtual void Initailize() override;
	virtual void DoStart() override;
	virtual void DoEnd() override;
	virtual void Update(float DeltaTime) override;
	

	UFUNCTION()
	void SequenceEnd();

private:
	ULevelSequence* ReadyLevelSequence;
};
