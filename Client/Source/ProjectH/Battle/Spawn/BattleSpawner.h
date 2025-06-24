// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "BattleSpawner.generated.h"

struct FSceneData;
struct FBattleStateParams;
class UHDCharacterData;
class AHDBattleSpawnPoint;
class UPaperZDAnimInstance;
class UPaperFlipbook;
class UHDAbilitySet;
class UHDAttributeSet;
 
UCLASS()
class PROJECTH_API UBattleSpawner : public UObject
{
	GENERATED_BODY()
public:
	void RegisterBPActor(TSubclassOf<AActor> BPActor, TSubclassOf<AActor> BPAIController);
	void OnInit(FSceneData* SceneDatas, TFunction<void(const FBattleStateParams)> AttackExecute);
	void GetBattlePoints();
	void SetBattleCharacters();
	void SetBattleMonsters(int32 GroupID);

	TArray<AActor*> GetCharacterActors()
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

private:
	UHDCharacterData* GetCharacterData();
	APawn* SpawnPawn(TSubclassOf<AActor> Actor, ECharType CharType, int32 SlotNo, FTransform SpawnPoint, FString PawnName, UHDAttributeSet* AttributeSet, TSubclassOf<UPaperZDAnimInstance> AnimInstance,
		TObjectPtr<UPaperFlipbook> Flipbook, TArray<TObjectPtr<UHDAbilitySet>> AbilitySets);

private:
	UPROPERTY()
	TArray<AHDBattleSpawnPoint*> PlayerSpawnPoints;
	UPROPERTY()
	TArray<AHDBattleSpawnPoint*> MonsterSpawnPoints;

	TSubclassOf<AActor> BattleActor;
	TSubclassOf<AActor> BattleAIActor;

	UPROPERTY()
	TMap<int32, AActor*> BattleCharDatas;
	UPROPERTY()
	TMap<int32, AActor*> BattleMonsterDatas;


	TFunction<void(const FBattleStateParams)> HandleAttackExecute;

};
