// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleTargetWidget.generated.h"

class UImage;

UCLASS()
class PROJECTH_API UBattleTargetWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UBattleTargetWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> TargetImg;

};
