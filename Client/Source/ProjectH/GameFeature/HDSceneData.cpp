// Fill out your copyright notice in the Description page of Project Settings.


#include "HDSceneData.h"
#include "ProjectH/LogChannels.h"
UMoveSessionData* UHDSceneData::CreateMoveSessionData(int32 TableNo)
{
	const FString ExperienceName = ExperinceID.PrimaryAssetName.ToString();

	UE_LOG(HDLog, Log, TEXT("[MoveSessionData] MapID : %s"), *MapID.ToString());

	UMoveSessionData* Result = NewObject<UMoveSessionData>();

	Result->MapID = MapID;
	UE_LOG(HDLog, Log, TEXT("[MoveSessionData] Result->MapID : %s"), *Result->MapID.ToString());
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);
	Result->ExtraArgs.Add(TEXT("SceneTableNo"), FString::FromInt(TableNo));
	return Result;
}
