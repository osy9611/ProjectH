// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Data/DataManagerSubsystem.h"
#include "ProjectH/Data/PlayerData/HDCharacterData.h"
#include "ProjectH/Data/PlayerData/HDPlayerDataSubsystem.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/Util/UtilFunc_Sprite.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "ProjectH/Battle/Spawn/HDBattleSpawnPoint.h"
#include "ProjectH/Battle/Data/BattleResourceData_AI.h"
#include "ProjectH/Battle/Data/BattleResourceData_Player.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet_Player.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet_Monster.h"
#include "ProjectH/UI/Battle/BattleTargetWidget.h"
#include "ProjectH/UI/Battle/BattleMonsterInfoWidget.h"
#include "ProjectH/Battle/AI/BattleAIController.h"


void UBattleSpawner::RegisterBPActor(TSubclassOf<AActor> BPActor, TSubclassOf<AActor> BPAIController)
{
	BattleActor = BPActor;
	BattleAIActor = BPAIController;
}

void UBattleSpawner::OnInit(FSceneData* SceneData, TFunction<void(const FBattleStateParams)> AttackExecute)
{
	if (!SceneData)
	{
		UE_LOG(HDLog, Warning, TEXT("[BattleSpawner] Init Fail SceneData is nullptr"));
		return;
	}

	if (SceneData->SceneType != ESceneType::Battle)
	{
		UE_LOG(HDLog, Warning, TEXT("[BattleSpawner] Init Fail Scene Type is Not Battle"));
		return;
	}


	if (AttackExecute)
		HandleAttackExecute = AttackExecute;

	GetBattlePoints();
	SetBattleCharacters();
	SetBattleMonsters(SceneData->SceneValue);
}

void UBattleSpawner::GetBattlePoints()
{
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHDBattleSpawnPoint::StaticClass(), SpawnPoints);

	for (AActor* SpawnPoint : SpawnPoints)
	{
		if (AHDBattleSpawnPoint* BattleSpawnPoint = Cast<AHDBattleSpawnPoint>(SpawnPoint))
		{
			if (BattleSpawnPoint->Side == ECharType::Character)
			{
				PlayerSpawnPoints.Add(BattleSpawnPoint);
			}

			if (BattleSpawnPoint->Side == ECharType::Monster)
			{
				MonsterSpawnPoints.Add(BattleSpawnPoint);
			}
		}
	}

	PlayerSpawnPoints.Sort([](const AHDBattleSpawnPoint& A, const AHDBattleSpawnPoint& B)
		{
			return A.SlotIndex < B.SlotIndex;
		});

	MonsterSpawnPoints.Sort([](const AHDBattleSpawnPoint& A, const AHDBattleSpawnPoint& B)
		{
			return A.SlotIndex < B.SlotIndex;
		});
}

void UBattleSpawner::SetBattleCharacters()
{
	UHDCharacterData* CharacterData = GetCharacterData();
	if (!CharacterData)
		return;

	TArray<const UHDAttributeSet_Player*> AttributeSets = CharacterData->GetPartySelectCharacters();
	AttributeSets.Add(CharacterData->CreateCharacterData(10001));
	AttributeSets.Add(CharacterData->CreateCharacterData(10002));

	int SlotIndex = 0;
	for (const UHDAttributeSet_Player* AttributeSet : AttributeSets)
	{
		//Find Table Data
		FCharacterData* CharData = AttributeSet->GetTableData<FCharacterData>();
		if (!CharData)
		{
			UE_LOG(HDLog, Log, TEXT("[BattleSubsystem] Character Table Data Not Found %s"), *AttributeSet->TableID);
			continue;
		}

		//Create Actor
		if (UBattleResourceData_Player* ResourceData = UtilFunc::LoadResource<UBattleResourceData_Player>(CharData->BattleResource))
		{
			APawn* SpawnedPawn = SpawnPawn(BattleActor, ECharType::Character, SlotIndex, PlayerSpawnPoints[SlotIndex]->GetTransform(), FString::FromInt(SlotIndex),
				const_cast<UHDAttributeSet_Player*>(AttributeSet), ResourceData->AnimInstance, ResourceData->PaperFilpbook, ResourceData->AbilitySets);

			SpawnedPawn->AttachToActor(PlayerSpawnPoints[SlotIndex], FAttachmentTransformRules::KeepWorldTransform);

			//왼쪽을 바라보도록 수정해야함
			FRotator NewRotation = FRotator(0.f, 180.f, 0.f);
			SpawnedPawn->SetActorRotation(NewRotation);

			if (UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(SpawnedPawn))
			{
				//Set Damage Widget
				BattleComp->ChachedWidgets("DamageWidget", nullptr);
			}

			BattleCharDatas.Add(SlotIndex, SpawnedPawn);

			SlotIndex++;
		}
	}
}

void UBattleSpawner::SetBattleMonsters(int32 GroupID)
{
	FMonsterGroupData* MonsterGroupData = UtilFunc_Data::GetTableData<FMonsterGroupData>(GetWorld(), FString::FromInt(GroupID));
	if (!MonsterGroupData)
	{
		UE_LOG(HDLog, Log, TEXT("[BattleSubsystem] MonsterGroup Not Found TableNo : %d"), GroupID);
		return;
	}

	int SlotIndex = 0;
	for (int32 MonsterID : MonsterGroupData->MonsterIDs)
	{
		//Find Table Data
		FMonsterData* MonsterData = UtilFunc_Data::GetTableData<FMonsterData>(GetWorld(), FString::FromInt(MonsterID));

		if (!MonsterData)
		{
			UE_LOG(HDLog, Log, TEXT("[BattleSubsystem] Monster Table Data Not Found %s"), *FString::FromInt(MonsterID));
			continue;
		}

		//Create Actor
		if (UBattleResourceData_AI* ResourceData = UtilFunc::LoadResource<UBattleResourceData_AI>(MonsterData->ResourcePath))
		{
			APawn* SpawnedPawn = SpawnPawn(BattleAIActor, ECharType::Monster, SlotIndex, MonsterSpawnPoints[SlotIndex]->GetTransform(), FString::FromInt(SlotIndex),
				nullptr, ResourceData->AnimInstance, ResourceData->PaperFilpbook, ResourceData->AbilitySets);

			//Create Attribute -> AttributeSet을 생성을 하기위함(추후 확장을 고려한다면 BattleComponent에서 해야함)
			UHDAttributeSet_Monster* AttributeSet = NewObject<UHDAttributeSet_Monster>(SpawnedPawn);
			AttributeSet->OnInit(FString::FromInt(MonsterID));

			if (UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(SpawnedPawn))
			{

				AController* Controller = SpawnedPawn->GetController();
				if (Controller)
				{
					ABattleAIController* AIController = Cast<ABattleAIController>(Controller);
					if (AIController)
					{
						AIController->OnInit(ResourceData);
					}
				}

				BattleComp->RegisterBattleData(AttributeSet);

				FVector SpriteSize = UtilFunc_Sprite::GetSpriteSize(SpawnedPawn);
				FVector CurrentLocation = SpawnedPawn->GetActorLocation();
				FVector NewLocation = CurrentLocation + SpriteSize;
				//Set Target Widget
				BattleComp->ChachedWidgets("TargetWidget", [&](UWidgetComponent* Result)
					{
						Result->AddLocalOffset(FVector(0.0f, 0.0f, NewLocation.Z));
						Result->SetVisibility(false);

						if (UBattleTargetWidget* TargetWidget = Cast<UBattleTargetWidget>(Result->GetWidget()))
						{
							TargetWidget->SetSizeBox(SpriteSize);
							TargetWidget->OnClickCallback = [this, SpawnedPawn]()
								{
									FBattleStateParams Params;
									Params.Objects = { SpawnedPawn };
									if (HandleAttackExecute)
										HandleAttackExecute(Params);
								};
						}
					});

				//Set Damage Widget
				BattleComp->ChachedWidgets("DamageWidget", [&](UWidgetComponent* Result)
					{
						Result->AddLocalOffset(FVector(0.0f, 0.0f, NewLocation.Z));
						Result->SetVisibility(false);
					});

				//Set Monster Info Widget
				BattleComp->ChachedWidgets("InfoWidget", [&](UWidgetComponent* Result)
					{
						UUserWidget* InfoWidget = Result->GetUserWidgetObject();
						if (InfoWidget)
						{
							if (UBattleMonsterInfoWidget* MonsterInfoWidget = Cast<UBattleMonsterInfoWidget>(InfoWidget))
							{
								MonsterInfoWidget->OnInit(*MonsterData);
							}
						}
					});
			}

			BattleMonsterDatas.Add(SlotIndex, SpawnedPawn);
			SlotIndex++;
		}
	}
}

UHDCharacterData* UBattleSpawner::GetCharacterData()
{
	UHDPlayerDataSubsystem* PlayerData = GetWorld()->GetGameInstance()->GetSubsystem<UHDPlayerDataSubsystem>();

	if (!PlayerData)
	{
		UE_LOG(HDLog, Error, TEXT("[BattleSubsystem] HDPlayerDataSubsystem is nullptr"));
		return nullptr;
	}

	UHDCharacterData* CharacterData = PlayerData->GetData<UHDCharacterData>("CharacterData");
	if (!CharacterData)
	{
		UE_LOG(HDLog, Log, TEXT("[BattleSubsystem] CharacterData is nullptr"));
	}

	return CharacterData;
}

APawn* UBattleSpawner::SpawnPawn(TSubclassOf<AActor> Actor, ECharType CharType, int32 SlotNo, FTransform SpawnPoint, FString PawnName, UHDAttributeSet* AttributeSet, TSubclassOf<UPaperZDAnimInstance> AnimInstance, TObjectPtr<UPaperFlipbook> Flipbook, TArray<TObjectPtr<UHDAbilitySet>> AbilitySets)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;


	if (APawn* SpawnedPawn = GetWorld()->SpawnActorDeferred<APawn>(Actor, SpawnPoint))
	{
		UGameplayStatics::FinishSpawningActor(SpawnedPawn, SpawnPoint);
		//SpawnedPawn->FinishSpawning(SpawnPoint);

		UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn);


		if (UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(SpawnedPawn))
		{
			if (AttributeSet != nullptr)
				BattleComp->RegisterBattleData(AttributeSet);
			BattleComp->SlotNo = SlotNo;
			BattleComp->CharType = CharType;
		}

		PawnExtComp->SetAnimationInstance(AnimInstance);
		PawnExtComp->SetFlipbook(Flipbook);


		AHDPlayerState* HDPS = SpawnedPawn->GetPlayerState<AHDPlayerState>();
		if (HDPS)
		{
			HDPS->RegisterAbilitySet(AbilitySets);
		}

		return SpawnedPawn;
	}
	return nullptr;
}
