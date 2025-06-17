// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "ProjectH/System/HDAssetManager.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Battle/State/BattleStateManager.h"
#include "BattleSubsystem.generated.h"

struct FSceneData;
class UHDCharacterData;
class AHDBattleSpawnPoint;
class UPaperZDAnimInstance;
class UPaperFlipbook;
class UBattleResourceData_Player;
class UHDAbilitySet;
class UHDAttributeSet;
class UTurnManager;
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

	void RegisterBPActor(TSubclassOf<AActor> BPActor, TSubclassOf<AActor> BPAIActor);
	void OnBattleInit(FString BattleSceneTableNo);
	void GetBattlePoints();
	void SetBattleCharacters();
	void SetBattleMonsters(int32 GroupID);

	//StateManager
	void InitState();
	void ChangeState(EBattleState Type);

	//TurnManager
	void RegisterTurn();
	UFUNCTION()
	void HandleTurnStart(UHDBattleComponent* BattleComp);

	//BattleInput
	void InitInput();

	TArray<AActor*> GetBattleActors()
	{
		TArray<AActor*> Result;
		BattleCharDatas.GenerateValueArray(Result);
		return Result;
	}

	TArray<AActor*> GetMonsterActors()
	{
		TArray<AActor*> Result;
		BattleMonsterDatas.GenerateValueArray(Result);
		return Result;
	}

	UTurnManager* GetTurnManager() { return TurnManager.Get(); }

	bool CheckBattleMode() { return IsBattle; }

private:
	UHDCharacterData* GetCharacterData();
	APawn* SpawnPawn(TSubclassOf<AActor> Actor, ECharType CharType,int32 SlotNo,FTransform SpawnPoint, FString PawnName, UHDAttributeSet* AttributeSet, TSubclassOf<UPaperZDAnimInstance> AnimInstance,
		TObjectPtr<UPaperFlipbook> Flipbook, TArray<TObjectPtr<UHDAbilitySet>> AbilitySets);

	template<typename T>
	T* GetResourceData(FString Path);

	FSceneData* GetSceneData(FString SceneID);

private:
	UPROPERTY()
	TArray<AHDBattleSpawnPoint*> PlayerSpawnPoints;
	UPROPERTY()
	TArray<AHDBattleSpawnPoint*> MonsterSpawnPoints;

	UPROPERTY()
	TObjectPtr<UTurnManager> TurnManager;
	UPROPERTY()
	TObjectPtr<UBattleStateManager> StateManager;
	UPROPERTY()
	TObjectPtr<UBattleInput> Input;

	TSubclassOf<AActor> BattleActor;
	TSubclassOf<AActor> BattleAIActor;

	UPROPERTY()
	TMap<int32, AActor*> BattleCharDatas;

	UPROPERTY()
	TMap<int32, AActor*> BattleMonsterDatas;


	bool IsBattle = false;
};

template<typename T>
T* UBattleSubsystem::GetResourceData(FString Path)
{

	if (Path.IsEmpty())
	{
		UE_LOG(HDLog, Log, TEXT("[BattleSubsystem] Resource Path is Empty"));
		return nullptr;
	}

	UHDAssetManager& AssetManager = UHDAssetManager::Get();
	UObject* Result = AssetManager.SynchronusLoadAsset(Path);
	if (!Result)
	{
		UE_LOG(HDLog, Log, TEXT("[BattleSubsystem] BattleResource Load Fail Can't Found Data : %s"), *Path);
		return nullptr;
	}

	return Cast<T>(Result);
}
