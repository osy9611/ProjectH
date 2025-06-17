// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonSessionSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/LocalPlayer.h"

FString UMoveSessionData::GetMapName()
{
	FAssetData MapAssetData;
	if (UAssetManager::Get().GetPrimaryAssetData(MapID, MapAssetData))
	{
		UE_LOG(LogTemp, Error, TEXT("[CommonSessionSubsystem] GetMapName PacakageName %s"), *MapAssetData.PackageName.ToString());
		return MapAssetData.PackageName.ToString();
	}

	return FString();
}

FString UMoveSessionData::CreateURL()
{
	FString CombinedExtraArgs;

	for (const auto& ExtraArg : ExtraArgs)
	{
		if (ExtraArg.Key.IsEmpty())
		{
			continue;
		}

		/**
		 * ?를 separate로 복수개의 ExtraArg를 추가함:
		 * - Key 값 유무에 따라, =(assignment)를 통해 알맞는 CmdArg를 생성
		 */
		if (ExtraArg.Value.IsEmpty())
		{
			CombinedExtraArgs += FString::Printf(TEXT("?%s"), *ExtraArg.Key);
		}
		else
		{
			CombinedExtraArgs += FString::Printf(TEXT("?%s=%s"), *ExtraArg.Key, *ExtraArg.Value);
		}
	}


	// Map 경로 앞에 추가하여, 최종 TravelURL 생성
	return FString::Printf(TEXT("%s%s"), *GetMapName(), *CombinedExtraArgs);
}

void UCommonSessionSubsystem::MoveSession(APlayerController* PlayerController, UMoveSessionData* SessionData)
{
	ULocalPlayer* LocalPlayer = PlayerController != nullptr ? PlayerController->GetLocalPlayer() : nullptr;

	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("[CommonSessionSubsystem] This PlayerController Is nullptr"));
		return;
	}

	GetWorld()->ServerTravel(SessionData->CreateURL());
}
