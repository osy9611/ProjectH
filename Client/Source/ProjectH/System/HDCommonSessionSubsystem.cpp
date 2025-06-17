// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCommonSessionSubsystem.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/GameFeature/HDSceneData.h"
#include "ProjectH/System/HDAssetManager.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Data/DataManagerSubsystem.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ModularGame/Sound/AudioSubsystem.h"

UHDCommonSessionSubsystem::UHDCommonSessionSubsystem()
{
}

void UHDCommonSessionSubsystem::MoveSeesionByTableID(int32 TableNo)
{
	FSceneData* SceneData = UtilFunc_Data::GetTableData<FSceneData>(GetWorld(), FString::FromInt(TableNo));
	if (!SceneData)
	{
		UE_LOG(HDLog, Log, TEXT("[HDCommonSessionSubsystem] SceneData Not Found Table ID : %d"), TableNo);
		return;
	}

	UAudioSubsystem* AudioManager = GetWorld()->GetGameInstance()->GetSubsystem<UAudioSubsystem>();
	if (AudioManager)
	{
		AudioManager->UnRegisterData();
	}

	UHDAssetManager& AssetManager = UHDAssetManager::Get();
	AssetManager.AsynchronusLoadAsset(SceneData->DataPath, [&,TableNo](UObject* result)
		{
			if (UHDSceneData* SceneData = Cast<UHDSceneData>(result))
			{
				UMoveSessionData* MoveSessionData = SceneData->CreateMoveSessionData(TableNo);

				UE_LOG(HDLog, Log, TEXT("[HDCommonSessionSubSystem] MoveSessionData Map : %s"), *MoveSessionData->MapID.ToString());

				if (MoveSessionData)
				{
					FString InURL = MoveSessionData->CreateURL();
					UE_LOG(HDLog, Log, TEXT("[HDCommonSessionSubSystem] CreateURL : %s"), *InURL);
					GetWorld()->ServerTravel(InURL);
				}
			}
		}
	);
}
