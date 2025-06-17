// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

struct FHDGameplayTags
{
	static const FHDGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddAllTags(UGameplayTagsManager& Manager);

	/*
	* �Ʒ��� GameplayTags�� �ʱ�ȭ ���� �ܰ踦 �ǹ��Ѵ�
	*  - GameInstance�� �ʱ�ȭ �������� UGameFrameworkComponentManager�� RegisterInitState�� ��ϵǾ� ���������� ������Ʈ �ȴ�.
	*  - �� �ʱ�ȭ GameplayTag�� ������ Actor ���̿� �����Ǹ�, GameFrameworkInitStateInterface ��ӹ��� Ŭ������ �ʱ�ȭ ���¸� State Machine�� ���� ���� ������
	*    �������̽��� �����Ѵ�
	*/
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;

	//InputTags
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Interaction;
	FGameplayTag InputTag_ClickTarget;

	//SkillTags
	FGameplayTag Battle_Skill1;
	FGameplayTag Battle_Skill2;
	FGameplayTag Battle_Skill3;
	FGameplayTag Battle_Skill4;


private:
	static FHDGameplayTags GameplayTags;
};