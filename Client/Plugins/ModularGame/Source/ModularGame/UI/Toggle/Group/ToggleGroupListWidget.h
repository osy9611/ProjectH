// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonListView.h"
#include "ToggleGroupListWidget.generated.h"

class UListView;
class UToggleWidget;
class UToggleData;

UCLASS()
class MODULARGAME_API UToggleGroupListWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UToggleGroupListWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;

	void AddToggleData(UToggleData* ToggleData);
	void SetToggleDatas(const TArray<UToggleData*>& ToggleDatas);
	void RemoveToggleData(UToggleData* ToggleData);
	void ClearToggleDatas();

	TArray<UObject*> GetAllListData() { return ListView->GetListItems(); }

	UFUNCTION()
	void OnSelectionChanged(UObject* SelectedItem);

	void HandleItemClick(UObject* ClickItem);

	UToggleData* GetToggleData(FString UniqueID);
	UToggleData* GetToggleData(UToggleData* ToggleData);

	void SelectFirst();
	void SelectItem(int32 Index);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UListView> ListView;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ToggleData)
	TSubclassOf<UToggleData> ToggleButtonData;
};
