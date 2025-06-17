// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "BattleState_CharTurn.generated.h"

class ULevelSequence;
class UBattleMonsterInfoWidget;
class UWidgetComponent;

UCLASS()
class PROJECTH_API UBattleState_CharTurn : public UBattleState
{
	GENERATED_BODY()
public:
	virtual void Initailize() override;
	virtual void DoStart() override;
	virtual void DoEnd() override;
	virtual void Update(float DeltaTime) override;

	void StartSelectSequence(int32 SlotNo);
	UFUNCTION()
	void EndSelectSequence();


	void ActiveMonsterTargetWiget();
	void DeActiveMonsterTargetWiget();
private:
	UPROPERTY()
	TArray<ULevelSequence*> Select_Sequences;

	UPROPERTY()
	TArray<ULevelSequence*> End_Sequences;

	UPROPERTY()
	TArray<UWidgetComponent*> MonsterTargetWidgets;
};
