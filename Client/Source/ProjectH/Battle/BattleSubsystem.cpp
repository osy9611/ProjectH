// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSubsystem.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/Battle/Turn/TurnManager.h"
#include "ProjectH/Battle/Input/BattleInput.h"

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
	BattleRandomSkillSelect.Initialize(FDateTime::Now().GetTicks() % INT32_MAX);
	StateManager->OnStart();
}

void UBattleSubsystem::OnEndBattle()
{
}

void UBattleSubsystem::OnBattleInit(FString BattleSceneTableNo, TSubclassOf<AActor> BPActor, TSubclassOf<AActor> BPAIActor)
{
	FSceneData* SceneData = UtilFunc_Data::GetTableData<FSceneData>(GetWorld(), BattleSceneTableNo);
	if (!SceneData)
	{
		UE_LOG(HDLog, Log, TEXT("[BattleSubsystem] Scene Data Not Found Table No : %s"), *BattleSceneTableNo);
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


	InitSpawner(SceneData, BPActor, BPAIActor);
	RegisterTurn();
	InitState();
	IsBattle = true;
}

void UBattleSubsystem::InitSpawner(FSceneData* SceneData, TSubclassOf<AActor> BPActor, TSubclassOf<AActor> BPAIController)
{
	UBattleSpawner* NewBattleSpawner = NewObject<UBattleSpawner>(this);
	BattleSpawner = NewBattleSpawner;

	BattleSpawner->RegisterBPActor(BPActor, BPAIController);

	BattleSpawner->OnInit(SceneData, [this](FBattleStateParams& Params)
		{
			HandleAttackExecute(Params);
		});
}

void UBattleSubsystem::RegisterTurn()
{
	UTurnManager* NewTurnManager = NewObject<UTurnManager>(this);
	TurnManager = NewTurnManager;

	TArray<AActor*> RegisterActors;
	TArray<AActor*> Temp;

	RegisterActors.Append(BattleSpawner->GetCharacterActors());
	RegisterActors.Append(BattleSpawner->GetMonsterActors());

	TurnManager->OnInitTurn(RegisterActors);
	TurnManager->OnTurnStart.AddUObject(this, &ThisClass::HandleTurnStart);
}

void UBattleSubsystem::ChangeState(EBattleState Type)
{
	if (!StateManager)
		return;

	StateManager->ChangeState(Type);
}

void UBattleSubsystem::HandleAttackExecute(FBattleStateParams& Target)
{
	if (!StateManager)
		return;

	StateManager->OnAttackExecute(Target);
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

int32 UBattleSubsystem::RandomSkillSelect(int32 Min, int32 Max)
{
	int32 Result = BattleRandomSkillSelect.RandRange(Min, Max);
	return Result;
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