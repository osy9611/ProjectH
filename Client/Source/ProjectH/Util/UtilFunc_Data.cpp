// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilFunc_Data.h"

FString UtilFunc_Data::GetCommonString(UWorld* World, FString TableID)
{
    if (!World)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc] Get Common String Fail World is nullptr"));
        return FString();
    }

    UDataManagerSubsystem* DataManager = World->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    if (!DataManager)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc] DataManagerSubsystem is nullptr"));
        return FString();
    }

    FString Result = DataManager->GetCommonString(TableID);

    return Result;
}

FString UtilFunc_Data::GetSkillString(UWorld* World, FString TableID)
{
    if (!World)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc] Get Common String Fail World is nullptr"));
        return FString();
    }

    UDataManagerSubsystem* DataManager = World->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    if (!DataManager)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc] DataManagerSubsystem is nullptr"));
        return FString();
    }

    FString Result = DataManager->GetSkillString(TableID);

    return Result;
}
