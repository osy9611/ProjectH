// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BattleWidget.generated.h"

struct FCharacterData;
class UVerticalBox;
class UBattleCharInfoWidget;
class UBattleTurnWidget;
class UBattleCharActiveWidget;

UCLASS()
class PROJECTH_API UBattleWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UBattleWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void OnInit();

	UFUNCTION(BlueprintCallable)
	void InitCharInfoWidget();
	void ResetCharInfoWidget();
	void UpdateCharInfoWidget();
	
	UFUNCTION(BlueprintCallable)
	void InitBattleTurnWidget();
	void UpdateBattleTurnWidget();

	void InitActiveBattleCharActiveWidget();
	void UpdateBattleCharActiveWidget(const FCharacterData& CharacterData);
	void ActiveBattleCharActiveWidget();
	void DeActiveBattleCharActiveWidget();
public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CharInfoRoot;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBattleTurnWidget> BattleTurn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBattleCharActiveWidget> BattleCharActive;

	UPROPERTY()
	TArray<UBattleCharInfoWidget*> CharInfos;
};
