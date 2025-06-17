// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/DataManagerSubsystem.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "ProjectH/Data/GenerateTableData.h"
class PROJECTH_API UtilFunc_Data
{
public:
	template<typename T = FTableRowBase>
	static T* GetTableData(UWorld* World, FString TableNo);

	static FString GetCommonString(UWorld* World, FString TableID);
	static FString GetSkillString(UWorld* World, FString TableID);
};

template<typename T>
T* UtilFunc_Data::GetTableData(UWorld* World, FString TableNo)
{
	if (!World)
	{
		UE_LOG(HDLog, Error, TEXT("[UtilFunc_Data] World is nullptr"));
		return nullptr;
	}

	UDataManagerSubsystem* DataManager = World->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
	if (!DataManager)
	{
		UE_LOG(HDLog, Error, TEXT("[UtilFunc_Data] DataManagerSubsystem is nullptr"));
		return nullptr;
	}

	T* TableData = DataManager->FindData<T>(TableNo);
	return TableData;
}
