// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSubsystem.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/AbilitySystem/HDAbilitySet.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Data/DataManagerSubsystem.h"
#include "ProjectH/Data/PlayerData/HDCharacterData.h"
#include "ProjectH/Data/PlayerData/HDPlayerDataSubsystem.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet_Player.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet_Monster.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Battle/Spawn/HDBattleSpawnPoint.h"
#include "ProjectH/Battle/Data/BattleResourceData_Player.h"
#include "ProjectH/Battle/Data/BattleResourceData_AI.h"
#include "ProjectH/Battle/Turn/TurnManager.h"
#include "ProjectH/Battle/Input/BattleInput.h"
#include "ProjectH/UI/Battle/BattleMonsterInfoWidget.h"
#include "ProjectH/UI/Battle/BattleTargetWidget.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/Util/UtilFunc_Sprite.h"
void UBattleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UBattleSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UBattleSubsystem::OnStartBattle()
{
	StateManager->OnStart();
}

void UBattleSubsystem::OnEndBattle()
{
}

void UBattleSubsystem::RegisterBPActor(TSubclassOf<AActor> BPActor, TSubclassOf<AActor> BPAIController)
{
	BattleActor = BPActor;
	BattleAIActor = BPAIController;
}

void UBattleSubsystem::OnBattleInit(FString BattleSceneData)
{
	FSceneData* SceneData = GetSceneData(BattleSceneData);
	if (!SceneData)
	{
		UE_LOG(HDLog, Log, TEXT("[BattleSubsystem] Scene Data Not Found Table No : %s"), *BattleSceneData);
		return;
	}

	if (SceneData->SceneType != ESceneType::Battle)
		return;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(nullptr); // UI 포커스 제거
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(InputMode);


	GetBattlePoints();
	SetBattleCharacters();
	SetBattleMonsters(SceneData->SceneValue);
	RegisterTurn();
	InitState();
	IsBattle = true;
}

void UBattleSubsystem::GetBattlePoints()
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

void UBattleSubsystem::SetBattleCharacters()
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
		if (UBattleResourceData_Player* ResourceData = GetResourceData<UBattleResourceData_Player>(CharData->BattleResource))
		{
			APawn* SpawnedPawn = SpawnPawn(BattleActor, ECharType::Character, SlotIndex, PlayerSpawnPoints[SlotIndex]->GetTransform(), FString::FromInt(SlotIndex),
				const_cast<UHDAttributeSet_Player*>(AttributeSet), ResourceData->AnimInstance, ResourceData->PaperFilpbook, ResourceData->AbilitySets);

			SpawnedPawn->AttachToActor(PlayerSpawnPoints[SlotIndex], FAttachmentTransformRules::KeepWorldTransform);

			//왼쪽을 바라보도록 수정해야함
			FRotator NewRotation = FRotator(0.f, 180.f, 0.f);
			SpawnedPawn->SetActorRotation(NewRotation);

			BattleCharDatas.Add(SlotIndex, SpawnedPawn);

			SlotIndex++;
		}
	}
}

void UBattleSubsystem::SetBattleMonsters(int32 GroupID)
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
		//Create Attribute 
		UHDAttributeSet_Monster* AttributeSet = NewObject<UHDAttributeSet_Monster>(this);
		AttributeSet->OnInit(FString::FromInt(MonsterID));

		//Find Table Data
		FMonsterData* MonsterData = AttributeSet->GetTableData<FMonsterData>();

		if (!MonsterData)
		{
			UE_LOG(HDLog, Log, TEXT("[BattleSubsystem] Monster Table Data Not Found %s"), *AttributeSet->TableID);
			continue;
		}

		//Create Actor
		if (UBattleResourceData_AI* ResourceData = GetResourceData<UBattleResourceData_AI>(MonsterData->ResourcePath))
		{
			APawn* SpawnedPawn = SpawnPawn(BattleAIActor, ECharType::Monster, SlotIndex, MonsterSpawnPoints[SlotIndex]->GetTransform(), FString::FromInt(SlotIndex),
				AttributeSet, ResourceData->AnimInstance, ResourceData->PaperFilpbook, ResourceData->AbilitySets);

			//Set Target Widget
			FVector SpriteSize  = UtilFunc_Sprite::GetSpriteSize(SpawnedPawn);
			FVector CurrentLocation = SpawnedPawn->GetActorLocation();
			FVector NewLocation = CurrentLocation + SpriteSize;
			UWidgetComponent* TargetWidgetComp = UtilFunc::GetActorComponent<UWidgetComponent>(SpawnedPawn,"TargetWidget");
			TargetWidgetComp->AddLocalOffset(FVector(0.0f,0.0f, NewLocation.Z));
			TargetWidgetComp->SetVisibility(false);

			if (UBattleTargetWidget* TargetWidget = Cast<UBattleTargetWidget>(TargetWidgetComp->GetWidget()))
			{
				TargetWidget->SetSizeBox(SpriteSize);
				TargetWidget->OnClickCallback = [this]()
					{
						HandleAttackExecute();
					};
			}

			//Set Info Widget
			UWidgetComponent* InfoWidgetComp = UtilFunc::GetActorComponent<UWidgetComponent>(SpawnedPawn, "InfoWidget");
			UUserWidget* InfoWidget = InfoWidgetComp->GetUserWidgetObject();
			if (InfoWidget)
			{
				if (UBattleMonsterInfoWidget* MonsterInfoWidget = Cast<UBattleMonsterInfoWidget>(InfoWidget))
				{
					MonsterInfoWidget->OnInit(*MonsterData);
				}				
			}

			BattleMonsterDatas.Add(SlotIndex, SpawnedPawn);
			SlotIndex++;
		}
	}
}

void UBattleSubsystem::RegisterTurn()
{
	UTurnManager* NewTurnManager = NewObject<UTurnManager>(this);
	TurnManager = NewTurnManager;

	TArray<AActor*> RegisterActors;
	TArray<AActor*> Temp;

	BattleCharDatas.GenerateValueArray(Temp);
	RegisterActors.Append(Temp);
	BattleMonsterDatas.GenerateValueArray(Temp);
	RegisterActors.Append(Temp);

	TurnManager->OnInitTurn(RegisterActors);
	TurnManager->OnTurnStart.AddUObject(this, &ThisClass::HandleTurnStart);
}

void UBattleSubsystem::ChangeState(EBattleState Type)
{
	if (!StateManager)
		return;

	StateManager->ChangeState(Type);
}

void UBattleSubsystem::HandleAttackExecute()
{
	if (!StateManager)
		return;

	StateManager->OnAttackExecute();
}

void UBattleSubsystem::CheckBattleState()
{
	if (!StateManager)
		return;

	if (IsWin())
	{
		StateManager->ChangeState(EBattleState::Win);
		return;
	}
	
	if (IsLose())
	{
		StateManager->ChangeState(EBattleState::Lose);
		return;
	}

	StateManager->CheckBattleState();
}

void UBattleSubsystem::InitState()
{
	UBattleStateManager* NewStateManager = NewObject<UBattleStateManager>(this);
	StateManager = NewStateManager;
	StateManager->OnInit();
}

void UBattleSubsystem::HandleTurnStart(UHDBattleComponent* BattleComp)
{
	if (!IsValid(BattleComp))
		return;

	if (BattleComp->CharType == ECharType::Character)
	{
		ChangeState(EBattleState::Char_Turn);
	}

	if (BattleComp->CharType == ECharType::Monster)
	{
		ChangeState(EBattleState::Monster_Turn);
	}
}

void UBattleSubsystem::InitInput()
{
	if (!IsBattle)
		return;

	UBattleInput* NewInput = NewObject<UBattleInput>(this);
	Input = NewInput;
	Input->OnInit();
}

bool UBattleSubsystem::IsWin()
{
	TArray<AActor*> MonsterActors = GetMonsterActors();

	bool isWin = true;
	for (AActor* Actor : MonsterActors)
	{
		UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(Actor);
		if (!BattleComp)
			continue;

		if (!BattleComp->CheckDead())
		{
			isWin = false;
		}
	}

	return isWin;
}

bool UBattleSubsystem::IsLose()
{
	TArray<AActor*> PlayerActors = GetCharacterActors();


	bool isLose = true;
	for (AActor* Actor : PlayerActors)
	{
		UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(Actor);
		if (!BattleComp)
			continue;

		if (!BattleComp->CheckDead())
		{
			isLose = false;
		}
	}
	return isLose;
}

UHDCharacterData* UBattleSubsystem::GetCharacterData()
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

APawn* UBattleSubsystem::SpawnPawn(TSubclassOf<AActor> Actor, ECharType CharType, int32 SlotNo, FTransform SpawnPoint, FString PawnName, UHDAttributeSet* AttributeSet,
	TSubclassOf<UPaperZDAnimInstance> AnimInstance, TObjectPtr<UPaperFlipbook> Flipbook, TArray<TObjectPtr<UHDAbilitySet>> AbilitySets)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(Actor, SpawnPoint, SpawnInfo))
	{
		SpawnedPawn->FinishSpawning(SpawnPoint);

		UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn);

		if (UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(SpawnedPawn))
		{
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

FSceneData* UBattleSubsystem::GetSceneData(FString SceneID)
{
	FSceneData* SceneData = UtilFunc_Data::GetTableData<FSceneData>(GetWorld(), SceneID);
	if (SceneData)
		return SceneData;

	return nullptr;
}

