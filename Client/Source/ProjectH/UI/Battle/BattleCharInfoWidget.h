// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BattleCharInfoWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UHDAttributeSet;
class UHDAttributeSet_Player;
UCLASS()
class PROJECTH_API UBattleCharInfoWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UBattleCharInfoWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void OnInit(UHDAttributeSet* AttributeSet);

	void UpdateHP();
	void UpdateMP();
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CharNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar>HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HPText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar>MPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MPText;

private:
	TWeakObjectPtr<UHDAttributeSet_Player> Attibute;
};
