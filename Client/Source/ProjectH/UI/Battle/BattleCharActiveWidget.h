// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "ModularGame/UI/Toggle/Group/ToggleGroupDefaultWidget.h"
#include "BattleCharActiveWidget.generated.h"

class UVerticalBox;
struct FCharacterData;

UCLASS()
class PROJECTH_API UBattleCharActiveWidget : public UToggleGroupDefaultWidget
{
	GENERATED_BODY()
public:
	UBattleCharActiveWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;	
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	virtual void OnSelectionChanged(UObject* SelectedItem) override;

	void GetToggles();
	void UpdateToggles(const FCharacterData& CharacterData);
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> ToggleRoot;

	FGameplayTag SelectedActiveTags;

	UPROPERTY()
	TArray<FGameplayTag> ActiveTags;
};
