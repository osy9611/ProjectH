// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "ModularGame/UI/Toggle/ToggleWidget.h"
#include "BattleCharActiveSelect.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PROJECTH_API UBattleCharActiveSelect : public UToggleWidget
{
	GENERATED_BODY()
public:
	UBattleCharActiveSelect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void OnActiveToggle(bool IsActive);

	void OnInit(FString ToggleText);
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text;

	FGameplayTag ActiveTag;
};
