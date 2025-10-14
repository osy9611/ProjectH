// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToggleWidget.generated.h"

class UImage;
class UOverlay;
	
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickEvent, UObject*, ClickObject);

UCLASS()
class MODULARGAME_API UToggleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UToggleWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnActiveToggle(bool IsActive);

	FOnClickEvent OnClickEvent;

	UFUNCTION(BlueprintCallable)
	void OnClick();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> ToggleOverlay;
};
