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

UCLASS()
class PROJECTH_API UHDAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UHDAttributeSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnInit(FString InitTableID);
	virtual void OnUpdateStatus();

	template<typename T = FTableRowBase>
	T* GetTableData() const;
public:
	ATTRIBUTE_ACCESSORS(UHDAttributeSet, HP);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData HP;

	ATTRIBUTE_ACCESSORS(UHDAttributeSet, MP);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MP;

	ATTRIBUTE_ACCESSORS(UHDAttributeSet, Speed);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Speed;

	//Table ID
	FString TableID;

	
};

template<typename T>
T* UHDAttributeSet::GetTableData() const
{
	return UtilFunc_Data::GetTableData<T>(GetWorld(), TableID);
}
