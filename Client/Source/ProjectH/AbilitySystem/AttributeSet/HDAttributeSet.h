// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Data/DataManagerSubsystem.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "HDAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

struct FCharacterData;
struct FSkillData;

UCLASS()
class PROJECTH_API UHDAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UHDAttributeSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static const TArray<FString> SkillTags;

	virtual void OnInit(FString InitTableID);
	virtual void OnUpdateStatus();
	virtual void SetSkillData(const TArray<int32>& SkillIDs);

	template<typename T = FTableRowBase>
	T* GetTableData() const;

	virtual TArray<FSkillData> GetSkillDatas();
	virtual FSkillData* GetSkillData(FGameplayTag SkillTag);

public:
	ATTRIBUTE_ACCESSORS(UHDAttributeSet, HP);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData HP;

	ATTRIBUTE_ACCESSORS(UHDAttributeSet, SP);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData SP;

	ATTRIBUTE_ACCESSORS(UHDAttributeSet, PATK);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData PATK;

	ATTRIBUTE_ACCESSORS(UHDAttributeSet, PDEF);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData PDEF;

	ATTRIBUTE_ACCESSORS(UHDAttributeSet, EATK);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData EATK;

	ATTRIBUTE_ACCESSORS(UHDAttributeSet, EDEF);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData EDEF;

	ATTRIBUTE_ACCESSORS(UHDAttributeSet, CRI);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CRI;

	ATTRIBUTE_ACCESSORS(UHDAttributeSet, Speed);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Speed;

	//Table ID
	FString TableID;
protected:

	TMap<FGameplayTag, FSkillData> SkillDatas;
};

template<typename T>
T* UHDAttributeSet::GetTableData() const
{
	return UtilFunc_Data::GetTableData<T>(GetWorld(), TableID);
}
