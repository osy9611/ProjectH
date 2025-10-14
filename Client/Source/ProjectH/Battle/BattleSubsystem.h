// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "ProjectH/System/HDAssetManager.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Battle/Spawn/BattleSpawner.h"
#include "ProjectH/Battle/State/BattleStateManager.h"
#include "ModularGame/Public/Camera/ModularCameraMode.h"
#include "GameplayAbilitySpec.h"
#include "BattleSubsystem.generated.h"

struct FSceneData;
class UTurnManager;
class UBattleSpawner;
class UBattleInput;
class UHDBattleCameraComponent;

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

	//BattleObserver
	UFUNCTION(BlueprintCallable)
	void RegisterBattleObserver(APawn* Pawn);

	//BattleCamera
	//턴제 전투의 경우 BattleObserver에 카메라가 있기 때문에 Subsystem을 통해서 관리를 해야한다.
	UFUNCTION(BlueprintCallable)
	void RegisterBattleCamera(UHDBattleCameraComponent* CameraMode);
	UHDBattleCameraComponent* GetBattleCam();
	void SetCameraMode(AActor* Target, TSubclassOf<UModularCameraMode> CameraMode, FGameplayAbilitySpecHandle& OwningSpecHandle, bool UseFovOffset);
	void ClearCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle, bool UseFovOffset);

	int32 RandomBattleSelect(int32 Min, int32 Max);

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
	TWeakObjectPtr<UHDBattleCameraComponent> BattleCam;

	UPROPERTY()
	TWeakObjectPtr<APawn> BattleObserver;

	UPROPERTY()
	TObjectPtr<UBattleInput> Input;

	bool IsBattle = false;

	UPROPERTY()
	FRandomStream BattleRandomSelect;
};