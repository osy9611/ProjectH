// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAttributeSet.h"
#include "GameplayTagsManager.h"

const TArray<FString> UHDAttributeSet::SkillTags = {
		"Battle.Skill1",
		"Battle.Skill2",
		"Battle.Skill3",
		"Battle.Skill4",
};

UHDAttributeSet::UHDAttributeSet(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UHDAttributeSet::OnInit(FString InitTableID)
{
	TableID = InitTableID;
}

void UHDAttributeSet::OnUpdateStatus()
{
	
}

void UHDAttributeSet::SetSkillData(const TArray<int32>& SkillIDs)
{
	int32 Index = 0;
	for (int32 SkillNo : SkillIDs)
	{
		FSkillData* SkillData = UtilFunc_Data::GetTableData<FSkillData>(GetWorld(), FString::FromInt(SkillNo));
		if (!SkillData)
			continue;

		FGameplayTag GameplayTag = FGameplayTag::RequestGameplayTag(FName(*SkillTags[Index]), false);
		if (!GameplayTag.IsValid())
		{

			continue;
		}
		SkillDatas.Add(GameplayTag, *SkillData);
		Index++;
	}
}

TArray<FSkillData> UHDAttributeSet::GetSkillDatas()
{
	TArray<FSkillData> Result; 
	SkillDatas.GenerateValueArray(Result);
	return Result;
}

FSkillData* UHDAttributeSet::GetSkillData(FGameplayTag SkillTag)
{
	return SkillDatas.Find(SkillTag);
}
