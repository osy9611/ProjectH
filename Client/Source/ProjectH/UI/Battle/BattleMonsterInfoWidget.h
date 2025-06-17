// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BattleMonsterInfoWidget.generated.h"

struct FMonsterData;

class UTextBlock;
class UProgressBar;

UCLASS()
class PROJECTH_API UBattleMonsterInfoWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UBattleMonsterInfoWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void OnInit(const FMonsterData MonsterData);
	void UpdateHP();
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ToughnessText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;
};