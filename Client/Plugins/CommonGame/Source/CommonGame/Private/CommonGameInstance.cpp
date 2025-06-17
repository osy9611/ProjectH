// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonGameInstance.h"
#include "CommonLocalPlayer.h"
#include "UIManagerSubsystem.h"

int32 UCommonGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
    int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, UserId);
    if (ReturnVal != INDEX_NONE)
    {
        UCommonLocalPlayer* CommonLocalPlayer = Cast<UCommonLocalPlayer>(NewPlayer);
        GetSubsystem<UUIManagerSubsystem>()->NotifyPlayerAdded(Cast<UCommonLocalPlayer>(NewPlayer));
    }

    return ReturnVal;
}

bool UCommonGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
    UCommonLocalPlayer* CommonLocalPlayer = Cast<UCommonLocalPlayer>(ExistingPlayer);
    GetSubsystem<UUIManagerSubsystem>()->NotifyPlayerDestroyed((CommonLocalPlayer));

    return Super::RemoveLocalPlayer(ExistingPlayer);
}
