// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAttributeSet_Player.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/Data/GenerateTableData.h"
UHDAttributeSet_Player::UHDAttributeSet_Player(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHDAttributeSet_Player::OnInit(FString InitTableID)
{
	Super::OnInit(InitTableID);
}

void UHDAttributeSet_Player::OnUpdateStatus()
{
	FCharacterData* TableData = UtilFunc_Data::GetTableData<FCharacterData>(GetWorld(), TableID);
	if (!TableData)
		return;

	FCharacterStatusData* StatusData = UtilFunc_Data::GetTableData<FCharacterStatusData>(GetWorld(), FString::FromInt(TableData->StatusNo));
	if (!StatusData)
		return;

	OriginHP = StatusData->HP;
	OriginMP = StatusData->MP;

	HP = StatusData->HP;
	MP = StatusData->MP;

	Speed = StatusData->Speed;
}
