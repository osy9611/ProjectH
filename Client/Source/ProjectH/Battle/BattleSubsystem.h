// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "ProjectH/System/HDAssetManager.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Battle/Spawn/BattleSpawner.h"
#include "ProjectH/Battle/State/BattleStateManager.h"
#include "BattleSubsystem.generated.h"

struct FSceneData;
class UTurnManager;
class UBattleSpawner;
class UBattleInput;

UCLASS()
class PROJECTH_API UBattleSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UBattleSubsystem, STATGROUP_Tickables);
	}

	void OnStartBattle();
	void OnEndBattle();

	void OnBattleInit(FString BattleSceneTableNo, TSubclassOf<AActor> BPActor, TSubclassOf<AActor> BPAIActor);

	//BattleSpawner
	void InitSpawner(FSceneData* SceneData, TSubclassOf<AActor> BPActor, TSubclassOf<AActor> BPAIController);

	//StateManager
	void InitState();
	void ChangeState(EBattleState Type);
	void HandleAttackExecute(FBattleStateParams& Target);
	void CheckBattleState();

	//TurnManager
	void RegisterTurn();
	UFUNCTION()
	void HandleTurnStart(UHDBattleComponent* BattleComp);

	//BattleInput
	void InitInput();

	int32 RandomSkillSelect(int32 Min, int32 Max);

	bool IsWin();
	bool IsLose();

	TArray<AActor*> GetCharacterActors()
	{
		return BattleSpawner->GetCharacterActors();
	}

	TArray<AActor*> GetMonsterActors()
	{
		return BattleSpawner->GetMonsterActors();
	}

	UTurnManager* GetTurnManager() { return TurnManager.Get(); }

	bool CheckBattleMode() { return IsBattle; }

private:
	UPROPERTY()
	TObjectPtr<UTurnManager> TurnManager;
	UPROPERTY()
	TObjectPtr<UBattleStateManager> StateManager;
	UPROPERTY()
	TObjectPtr<UBattleSpawner> BattleSpawner;

	UPROPERTY()
	TObjectPtr<UBattleInput> Input;

	bool IsBattle = false;

	UPROPERTY()
	FRandomStream BattleRandomSkillSelect;
};