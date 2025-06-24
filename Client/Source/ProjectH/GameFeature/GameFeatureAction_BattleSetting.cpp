// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction_BattleSetting.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/GameModes/HDExperienceManagerComponent.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "ProjectH/Data/DataManagerSubsystem.h"
void UGameFeatureAction_BattleSetting::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

void UGameFeatureAction_BattleSetting::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();

	if (!World)
	{
		UE_LOG(HDLog, Log, TEXT("[GameFeatureAction_BattleSetting] World Is nullptr"));
		return;
	}

	SetBattle(World);
}

void UGameFeatureAction_BattleSetting::SetBattle(UWorld* World)
{
	if (!World)
		return;
	UBattleSubsystem* BattleSubsystem = World->GetSubsystem<UBattleSubsystem>();

	if (!BattleSubsystem)
	{
		UE_LOG(HDLog, Error, TEXT("[GameFeatureAction_BattleSetting] BattleSubsystem Is nullptr"));
		return;
	}

	AGameStateBase* GameState = World->GetGameState();
	if (!GameState)
	{
		UE_LOG(HDLog, Error, TEXT("[GameFeatureAction_BattleSetting] GameState is nullptr"));
		return;
	}
	UHDExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UHDExperienceManagerComponent>();

	BattleSubsystem->OnBattleInit(ExperienceManagerComponent->CurrentSceneTableNo, BattleChar, BattleAI);
	BattleSubsystem->OnStartBattle();
}

