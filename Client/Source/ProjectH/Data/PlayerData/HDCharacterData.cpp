// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCharacterData.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet_Player.h"

UHDAttributeSet_Player* UHDCharacterData::CreateCharacterData(int32 TableID)
{
	if (TObjectPtr<UHDAttributeSet_Player>* Data = CharacterDatas.Find(FString::FromInt(TableID)))
	{
		return *Data;
	}

	FCharacterData* CharData = UtilFunc_Data::GetTableData<FCharacterData>(GetWorld(), FString::FromInt(TableID));
	if (!CharData)
	{
		UE_LOG(HDLog, Log, TEXT("[HDCharacterData] CreateCharacterData Find CharacterData Not Found TableID %d"), TableID);
		return nullptr;
	}

	UHDAttributeSet_Player* NewAttributeSet = NewObject<UHDAttributeSet_Player>(this);
	NewAttributeSet->OnInit(FString::FromInt(TableID));
	CharacterDatas.Add(FString::FromInt(TableID), NewAttributeSet);

	return NewAttributeSet;
}

UHDAttributeSet_Player* UHDCharacterData::GetCharacterData(int32 TableID)
{
	if (TObjectPtr<UHDAttributeSet_Player>* Data = CharacterDatas.Find(FString::FromInt(TableID)))
	{
		return *Data;
	}

	return nullptr;
}

TArray<const UHDAttributeSet_Player*> UHDCharacterData::GetPartySelectCharacters()
{
	TArray<const UHDAttributeSet_Player*> Result;

	for (FString TableID : PartySelectCharacters)
	{
		UHDAttributeSet_Player* AttributeSet = GetCharacterData(FCString::Atoi(*TableID));
		if (!AttributeSet)
			continue;

		Result.Add(AttributeSet);
	}

	return Result;
}
