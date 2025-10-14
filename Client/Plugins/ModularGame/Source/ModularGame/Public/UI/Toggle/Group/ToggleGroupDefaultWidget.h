// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToggleGroupDefaultWidget.generated.h"

class UToggleWidget;
class UToggleData;

UCLASS()
class MODULARGAME_API UToggleGroupDefaultWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UToggleGroupDefaultWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;

	virtual void AddToggleWidget(UToggleWidget* ToggleWidget);
	virtual void RemoveToggleWidget(UToggleWidget* ToggleWidget);
	virtual void ClearToggleWidget();

	UFUNCTION()
	virtual void OnSelectionChanged(UObject* SelectedItem);
	void HandleItemClick(UObject* ClickItem);

	UToggleWidget* GetToggleWiget(UToggleWidget* ToggleWidget);

public:
	UPROPERTY()
	TArray<UToggleWidget*> Toggles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ToggleData)
	TSubclassOf<UToggleData> ToggleButtonData;
};
