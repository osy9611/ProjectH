// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameplayTags.h"
#include "LogChannels.h"
#include "GameplayTagsManager.h"

FHDGameplayTags FHDGameplayTags::GameplayTags;

void FHDGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(Manager);
}

void FHDGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}

void FHDGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InitState_Spawned, "InitState.Spawned", "1: Actor/Component has initially spawned and can be extended");
	AddTag(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization");
	AddTag(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	AddTag(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");

	//Input Tags
	AddTag(InputTag_Move, "InputTag.Move", "");
	AddTag(InputTag_Interaction, "InputTag.Interaction", "");
	AddTag(InputTag_ClickTarget, "InputTag.ClickTarget", "");

	//Skill Tag
	AddTag(Battle_Skill1, "Battle.Skill1", "");
	AddTag(Battle_Skill2, "Battle.Skill2", "");
	AddTag(Battle_Skill3, "Battle.Skill3", "");
	AddTag(Battle_Skill4, "Battle.Skill4", "");
}
