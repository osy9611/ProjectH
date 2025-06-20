// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BattleStateManager.generated.h"

class UBattleState;
struct FBattleStateParams;

UENUM()
enum class EBattleState : uint8
{
	Ready,
	Play,
	Char_Turn,
	Monster_Turn,
	Win,
	Lose,
	End
};

UCLASS()
class PROJECTH_API UBattleStateManager : public UObject
{
	GENERATED_BODY()
public:
	UBattleStateManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void RegisterState();
	void OnInit();
	void OnStart();
	void OnAttackExecute(const FBattleStateParams& Params);
	void CheckBattleState();

	void UpdateState(float DeltaTime);
	void ChangeState(EBattleState NextState);
private:
	EBattleState CurrentState;
	EBattleState PrevState;

	UPROPERTY()
	TMap<EBattleState, UBattleState*> States;
};
