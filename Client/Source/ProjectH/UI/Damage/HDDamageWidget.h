// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HDDamageWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECTH_API UHDDamageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UHDDamageWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void StartDamageWidget(float DamageAmount);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayDamageWidget();

	UFUNCTION(BlueprintCallable)
	void EndDamageWidget();
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;
};
