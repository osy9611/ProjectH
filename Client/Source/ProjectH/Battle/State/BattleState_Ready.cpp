// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleState_Ready.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/UI/Battle/BattleWidget.h"
#include "ProjectH/UI/HDUIManagerSubsystem.h"
#include "ProjectH/UI/Container/WidgetContainer.h"
void UBattleState_Ready::Initailize()
{
	Super::Initailize();

	ULevelSequence* BattleReadySequence = UtilFunc::LoadResource<ULevelSequence>("/BattleGame/Sequence/LS_BattleEnter.LS_BattleEnter");
	ReadyLevelSequence = BattleReadySequence;
}

void UBattleState_Ready::DoStart()
{
	Super::DoStart();

	BattleWidget->OnInit();

	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	ALevelSequenceActor* OutActor = nullptr;

	ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), ReadyLevelSequence, PlaybackSettings, OutActor);


	if (!SequencePlayer || !OutActor)
	{
		return;
	}

	SequencePlayer->OnFinished.AddDynamic(this, &ThisClass::SequenceEnd);
	SequencePlayer->Play();

}

void UBattleState_Ready::DoEnd()
{
	Super::DoEnd();
}

void UBattleState_Ready::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
}

void UBattleState_Ready::SequenceEnd()
{
	BattleWidget->UpdateCharInfoWidget();

	ChangeState(EBattleState::Play);
}
