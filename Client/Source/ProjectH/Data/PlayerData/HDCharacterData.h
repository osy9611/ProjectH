// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/Data/ModularUserData.h"
#include "HDCharacterData.generated.h"

class UHDAttributeSet_Player;
UCLASS()
class PROJECTH_API UHDCharacterData : public UModularUserData
{
	GENERATED_BODY()
public:
	UHDAttributeSet_Player* CreateCharacterData(int32 TableID);
	UHDAttributeSet_Player* GetCharacterData(int32 TableID);

	TArray<const UHDAttributeSet_Player*> GetPartySelectCharacters();
private:
	UPROPERTY()
	TArray<FString> PartySelectCharacters;

	UPROPERTY()
	TMap<FString, TObjectPtr<UHDAttributeSet_Player>> CharacterDatas;
};
