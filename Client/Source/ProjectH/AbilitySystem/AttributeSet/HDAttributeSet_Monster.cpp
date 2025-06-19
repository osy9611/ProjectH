// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAttributeSet_Monster.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Util/UtilFunc_Data.h"

UHDAttributeSet_Monster::UHDAttributeSet_Monster(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHDAttributeSet_Monster::OnInit(FString InitTableID)
{
	Super::OnInit(InitTableID);

	OnUpdateStatus();

	FMonsterData* MonsterData = UtilFunc_Data::GetTableData<FMonsterData>(GetWorld(), TableID);
	if (!MonsterData)
		return;
	
	SetSkillData(MonsterData->SkillNos);
}

void UHDAttributeSet_Monster::OnUpdateStatus()
{
	FMonsterData* MonsterData = UtilFunc_Data::GetTableData<FMonsterData>(GetWorld(), TableID);

	if (!MonsterData)
		return;

	FMonsterStatusData* StatusData = UtilFunc_Data::GetTableData<FMonsterStatusData>(GetWorld(), FString::FromInt(MonsterData->StatusNo));
	if (!StatusData)
		return;

	HP = StatusData->HP; 
	PATK = StatusData->PATK;
	PDEF = StatusData->PDEF;
	EATK = StatusData->EATK;
	EDEF = StatusData->EDEF;
	CRI = StatusData->CRI;
	Speed = StatusData->Speed;
}

