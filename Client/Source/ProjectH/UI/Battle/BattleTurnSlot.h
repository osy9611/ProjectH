// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleTurnSlot.generated.h"

class UImage;
class UPaperSprite;
UCLASS()
class PROJECTH_API UBattleTurnSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	UBattleTurnSlot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void OnInit(UPaperSprite* PaperSprite);
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CharImage;
};
