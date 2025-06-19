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

	FCharacterData* TableData = UtilFunc_Data::GetTableData<FCharacterData>(GetWorld(), TableID);
	if (!TableData)
		return;
	SetSkillData(TableData->SkillIDs);
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
	OriginSP = StatusData->SP;

	HP = StatusData->HP;
	SP = StatusData->SP;

	PATK = StatusData->PATK;
	PDEF = StatusData->PDEF;
	EATK = StatusData->EATK;
	EDEF = StatusData->EDEF;
	CRI = StatusData->CRI;
	Speed = StatusData->Speed;
}
