// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BattleMonsterInfoWidget.generated.h"

struct FMonsterData;

class UTextBlock;
class UProgressBar;
class UHDAbilitySystemComponent;
class UHDMVVM_MonsterInfo;

UCLASS()
class PROJECTH_API UBattleMonsterInfoWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UBattleMonsterInfoWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void OnInit(UHDAbilitySystemComponent* ASC,const FMonsterData& MonsterData);

	void HandleUpdateHPBar(UObject* Object, UE::FieldNotification::FFieldId FieldId);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ToughnessText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

private:
	UPROPERTY()
	TObjectPtr<UHDMVVM_MonsterInfo> MonsterInfo;
};