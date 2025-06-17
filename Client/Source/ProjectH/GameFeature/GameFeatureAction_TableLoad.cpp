// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction_TableLoad.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/DataManagerSubsystem.h"

void UGameFeatureAction_TableLoad::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

void UGameFeatureAction_TableLoad::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{

	UWorld* World = WorldContext.World();

	if (!World)
	{
		UE_LOG(HDLog, Log, TEXT("[GameFeatureAction_TableLoad] World Is nullptr"));
		return;
	}

	LoadTableData(World);
}

void UGameFeatureAction_TableLoad::LoadTableData(UWorld* World)
{
	if (UDataManagerSubsystem* DataManager = World->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>())
	{
		if (!DataManager->IsTableDataLoaded())
		{
			UE_LOG(HDLog, Log, TEXT("[GameFeatureAction_TableLoad] Load Table Data"));
			DataManager->LoadAllData();
		}
		else
		{
			UE_LOG(HDLog, Log, TEXT("[GameFeatureAction_TableLoad] ReLoad Table Data"));
			DataManager->ReloadAllData();
		}

	}
}
