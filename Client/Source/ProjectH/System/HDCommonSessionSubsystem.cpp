// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCommonSessionSubsystem.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/GameFeature/HDSceneData.h"
#include "ProjectH/System/HDAssetManager.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Data/DataManagerSubsystem.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ModularGame/Public/Sound/AudioSubsystem.h"
#include "ModularGame/Public/NetWork/HTTP/HTTPHandler.h"

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

void UHDCommonSessionSubsystem::CreateHttpObject()
{
	Handler = NewObject<UHTTPHandler>();
	Handler->Init(GetWorld());
}

void UHDCommonSessionSubsystem::OnCall_Get()
{
	if (!Handler)
		return;
	FHTTPRequestOption Option;
	Handler->SendGetRequest(TEXT("http://127.0.0.1:8000/items"), Option,
		[](const FJsonObject& Json, bool bSuccess)
		{
			if (bSuccess)
			{

			}
		});
}

void UHDCommonSessionSubsystem::OnCall_Post()
{
	if (!Handler)
		return;

	TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetStringField("name", "Sword of Dawn");
	Obj->SetStringField("description", "Legendary blade");
	Obj->SetNumberField("price", 999.9);

	FHTTPRequestOption Option;
	Option.MaxRetry = 2;
	Option.RetryDelaySeconds = 1;
	Handler->SendPostRequest(TEXT("http://127.0.0.1:8000/items/"), Option, Obj,
		[](const FJsonObject& Json, bool bSuccess)
		{
			if (bSuccess)
			{

			}
		});
}
