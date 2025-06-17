// Fill out your copyright notice in the Description page of Project Settings.


#include "HDPlayerDataSubsystem.h"
#include "HDCharacterData.h"

void UHDPlayerDataSubsystem::RegisterUserData()
{
	UserDatas.Add("CharacterData", NewObject<UHDCharacterData>(this));
}

void UHDPlayerDataSubsystem::RegisterListener()
{
}
