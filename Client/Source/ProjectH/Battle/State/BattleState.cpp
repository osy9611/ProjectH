// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleState.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/Battle/Turn/TurnManager.h"
#include "ProjectH/UI/HDUIManagerSubsystem.h"
#include "ProjectH/UI/Battle/BattleWidget.h"
UBattleState::UBattleState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UBattleState::Initailize()
{
	UHDUIManagerSubsystem* UIManager = GetWorld()->GetGameInstance()->GetSubsystem<UHDUIManagerSubsystem>();
	if (!UIManager)
	{
		UE_LOG(HDLog, Error, TEXT("[BattleState] UIManager is nullptr"));
		return;
	}

	BattleWidget = UIManager->FindWidget<UBattleWidget>(EUIType::Battle);

	UBattleSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
	if (!BattleSubsystem)
	{
		UE_LOG(HDLog, Error, TEXT("[BattleState] BattleSubsystem is nullptr"));
		return;
	}
	
	TurnManager = BattleSubsystem->GetTurnManager();
}

void UBattleState::ChangeState(EBattleState State)
{
	UBattleSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
	if (!BattleSubsystem)
	{
		UE_LOG(HDLog, Error, TEXT("[BattleState] BattleSubsystem is nullptr"));
		return;
	}

	BattleSubsystem->ChangeState(State);
}
