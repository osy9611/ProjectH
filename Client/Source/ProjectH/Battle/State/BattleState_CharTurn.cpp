// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleState_CharTurn.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/HDGameplayTags.h"
#include "ProjectH/Util/HDMessageExtensions.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Util/UtilFunc_UI.h"
#include "ProjectH/Battle/Turn/TurnManager.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/UI/Battle/BattleWidget.h"


void UBattleState_CharTurn::RegisterListener()
{
	const FHDGameplayTags& GameplayTags = FHDGameplayTags::Get();

	UtilFunc::RegisterNotifyListener(GetWorld(), GameplayTags.Battle_SelectActive, this, &ThisClass::Request_SelectActive);
}

void UBattleState_CharTurn::Request_SelectActive(FGameplayTag InChannel, const FBattleEventSelectActivate& InMessage)
{
	ActiveMonsterTargetWiget();
	CurrentSelectActive = InMessage.SelectTag;
}

void UBattleState_CharTurn::Initailize()
{
	Super::Initailize();

	Select_Sequences.Add(UtilFunc::LoadResource<ULevelSequence>("/BattleGame/Sequence/LS_CharSelect1.LS_CharSelect1"));
	Select_Sequences.Add(UtilFunc::LoadResource<ULevelSequence>("/BattleGame/Sequence/LS_CharSelect2.LS_CharSelect2"));
	Select_Sequences.Add(UtilFunc::LoadResource<ULevelSequence>("/BattleGame/Sequence/LS_CharSelect3.LS_CharSelect3"));
	Select_Sequences.Add(UtilFunc::LoadResource<ULevelSequence>("/BattleGame/Sequence/LS_CharSelect4.LS_CharSelect4"));

	UBattleSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
	if (!BattleSubsystem)
	{
		UE_LOG(HDLog, Error, TEXT("[BattleState_CharTurn] BattleSubsystem is nullpt "));
		return;
	}

	TArray<AActor*> MonsterActors= BattleSubsystem->GetMonsterActors();
	for (AActor* Actor : MonsterActors)
	{
		UWidgetComponent* MonsterWidgetComp = UtilFunc::GetActorComponent<UWidgetComponent>(Actor, "TargetWidget");
		MonsterTargetWidgets.Add(MonsterWidgetComp);
	}

	RegisterListener();
}

void UBattleState_CharTurn::DoStart()
{
	Super::DoStart();

	if (!TurnManager.IsValid())
		return;

	UHDBattleComponent* BattleComp = TurnManager->GetCurrentActor();
	if (!BattleComp)
		return;

	StartSelectSequence(BattleComp->SlotNo);
}

void UBattleState_CharTurn::DoEnd()
{
	SequenceEndCallback = nullptr;
	Super::DoEnd();
}

void UBattleState_CharTurn::DoExecute(const FBattleStateParams& Params)
{
	UHDBattleComponent* BattleComp = TurnManager->GetCurrentActor();
	if (!BattleComp)
		return;

	if (BattleComp->CharType != ECharType::Character)
		return;

	BattleComp->ProcessAbility_Skill(CurrentSelectActive, Params);
	BattleWidget->DeActiveBattleCharActiveWidget();

	DeActiveMonsterTargetWiget();
}

void UBattleState_CharTurn::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
}

void UBattleState_CharTurn::HandleEndSequence(TFunction<void()> Callback)
{
	UHDBattleComponent* BattleComp = TurnManager->GetCurrentActor();
	if (!BattleComp)
		return;
	
	int32 SlotNo = BattleComp->SlotNo;

	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	PlaybackSettings.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceKeepState;
	ALevelSequenceActor* OutActor = nullptr;

	ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), Select_Sequences[SlotNo], PlaybackSettings, OutActor);

	if (!SequencePlayer || !OutActor)
	{
		return;
	}

	SequenceEndCallback = Callback;

	SequencePlayer->OnFinished.AddDynamic(this, &ThisClass::FinishEndSelectSequence);
	SequencePlayer->PlayReverse();
}


void UBattleState_CharTurn::StartSelectSequence(int32 SlotNo)
{
	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	PlaybackSettings.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceKeepState;
	ALevelSequenceActor* OutActor = nullptr;

	ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), Select_Sequences[SlotNo], PlaybackSettings, OutActor);

	if (!SequencePlayer || !OutActor)
	{
		return;
	}

	SequencePlayer->OnFinished.AddDynamic(this, &ThisClass::FinishStartSelectSequence);
	SequencePlayer->Play();
}

void UBattleState_CharTurn::FinishStartSelectSequence()
{
	BattleWidget->ActiveBattleCharActiveWidget();
}

void UBattleState_CharTurn::FinishEndSelectSequence()
{
	if (SequenceEndCallback)
		SequenceEndCallback();
}

void UBattleState_CharTurn::ActiveMonsterTargetWiget()
{
	for (UWidgetComponent* MonsterInfoWidget : MonsterTargetWidgets)
	{
		if (!MonsterInfoWidget)
			continue;

		MonsterInfoWidget->SetVisibility(true);
	}
}

void UBattleState_CharTurn::DeActiveMonsterTargetWiget()
{
	for (UWidgetComponent* MonsterInfoWidget : MonsterTargetWidgets)
	{
		if (!MonsterInfoWidget)
			continue;

		MonsterInfoWidget->SetVisibility(false);
	}
}
