// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilFunc_Data.h"

FString UtilFunc_Data::GetCommonString(UWorld* World, FString TableID)
{
    if (!World)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc_Data] Get Common String Fail World is nullptr"));
        return FString();
    }

    UDataManagerSubsystem* DataManager = World->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    if (!DataManager)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc_Data] DataManagerSubsystem is nullptr"));
        return FString();
    }

    FString Result = DataManager->GetCommonString(TableID);

    return Result;
}

FString UtilFunc_Data::GetSkillString(UWorld* World, FString TableID)
{
    if (!World)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc_Data] Get Common String Fail World is nullptr"));
        return FString();
    }

    UDataManagerSubsystem* DataManager = World->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    if (!DataManager)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc_Data] DataManagerSubsystem is nullptr"));
        return FString();
    }

    FString Result = DataManager->GetSkillString(TableID);

    return Result;
}

float UtilFunc_Data::GetBattleConstValue(UWorld* World,EBattleConstType BattleConstType)
{
    if (!World)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc_Data] Get Common String Fail World is nullptr"));
        return 0.0f;
    }

    UDataManagerSubsystem* DataManager = World->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    if (!DataManager)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc_Data] DataManagerSubsystem is nullptr"));
        return 0.0f;
    }

    const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EBattleConstType"), true);
    if (!EnumPtr)
    {
        UE_LOG(HDLog, Error, TEXT("[UtilFunc_Data] EnumPtr Not Found"));
        return 0.0f;
    }

    FString TableID = EnumPtr->GetNameStringByValue(static_cast<int64>(BattleConstType));

    FBattleConst* BattleConst = DataManager->FindData<FBattleConst>(TableID);

    return BattleConst->value;
}
