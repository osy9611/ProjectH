// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleGroupListWidget.h"
#include "ModularGame/Log/ModularLogChannel.h"
#include "ModularGame/UI/Toggle/ToggleWidget.h"
#include "ModularGame/UI/Toggle/Data/ToggleData.h"

UToggleGroupListWidget::UToggleGroupListWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UToggleGroupListWidget::NativeConstruct()
{
	ListView->OnItemSelectionChanged().AddUObject(this, &ThisClass::OnSelectionChanged);
	ListView->OnItemClicked().AddUObject(this, &ThisClass::HandleItemClick);
}

void UToggleGroupListWidget::AddToggleData(UToggleData* ToggleData)
{
	if (!ToggleData)
	{
		UE_LOG(Modular, Error, TEXT("[ToggleGroupListWidget] Add Toggle Data Fail ToggleData is nullptr"));
		return;
	}

	if (!ListView)
	{
		UE_LOG(Modular, Error, TEXT("[ToggleGroupListWidget] Add Toggle Data Fail ListView is nullptr"));
		return;
	}

	ListView->AddItem(Cast<UObject>(ToggleData));
}

void UToggleGroupListWidget::SetToggleDatas(const TArray<UToggleData*>& ToggleDatas)
{
	if (!ListView)
	{
		UE_LOG(Modular, Error, TEXT("[ToggleGroupListWidget] Set Toggle Datas Fail ListView is nullptr"));
		return;
	}

	ListView->SetListItems(ToggleDatas);
}

void UToggleGroupListWidget::RemoveToggleData(UToggleData* ToggleData)
{
	if (!ToggleData)
	{
		UE_LOG(Modular, Error, TEXT("[ToggleGroupListWidget] Remove Toggle Data Fail ToggleData is nullptr"));
		return;
	}

	if (!ListView)
	{
		UE_LOG(Modular, Error, TEXT("[ToggleGroupListWidget] Remove Toggle Data Fail ListView is nullptr"));
		return;
	}

	TArray<UObject*> Items = ListView->GetListItems();

	if (GetToggleData(ToggleData))
	{
		ListView->RemoveItem(Cast<UObject>(ToggleData));
	}
}

void UToggleGroupListWidget::ClearToggleDatas()
{
	if (!ListView)
	{
		UE_LOG(Modular, Error, TEXT("[ToggleGroupListWidget] Clear Toggle Data Fail ListView is nullptr"));
		return;
	}

	ListView->ClearListItems();
}

void UToggleGroupListWidget::OnSelectionChanged(UObject* SelectedItem)
{
	TArray<UObject*> Items = ListView->GetListItems();
	for (UObject* Item : Items)
	{
		if (UToggleData* Data = Cast<UToggleData>(Item))
		{
			bool bIsActive = (Item == SelectedItem);
			Data->IsToggled = bIsActive;

			if (UToggleWidget* Widget = ListView->GetEntryWidgetFromItem<UToggleWidget>(Item))
			{
				Widget->OnActiveToggle(bIsActive);
			}
		}
	}
}

void UToggleGroupListWidget::HandleItemClick(UObject* ClickItem)
{
	if (!ClickItem)
	{
		return;
	}

	ListView->ClearSelection();
	ListView->SetSelectedItem(ClickItem);
}

UToggleData* UToggleGroupListWidget::GetToggleData(FString UniqueID)
{
	if (!ListView)
	{
		return nullptr;
	}

	TArray<UObject*>Items = ListView->GetListItems();
	UObject** FoundData = Items.FindByPredicate([UniqueID](UObject* Obj)
		{
			if (UToggleData* Data = Cast<UToggleData>(Obj))
			{
				return Data->UniqueID == UniqueID;
			}
			return false;
		});

	if (FoundData)
		return Cast<UToggleData>(*FoundData);

	return nullptr;
}

UToggleData* UToggleGroupListWidget::GetToggleData(UToggleData* ToggleData)
{
	if (!ListView)
	{
		return nullptr;
	}

	TArray<UObject*>Items = ListView->GetListItems();
	UObject** FoundData = Items.FindByPredicate([ToggleData](UObject* Obj)
		{
			return Cast<UObject>(ToggleData) == Obj;
		});

	if (FoundData)
		return Cast<UToggleData>(*FoundData);

	return nullptr;
}


void UToggleGroupListWidget::SelectFirst()
{
	ListView->ClearSelection();
	ListView->ScrollIndexIntoView(0);
	ListView->SetSelectedIndex(0);

	UObject* FirstItem = ListView->GetItemAt(0);
	if (FirstItem)
	{
		ListView->OnItemClicked().Broadcast(FirstItem);
	}
}

void UToggleGroupListWidget::SelectItem(int32 Index)
{
	UObject* Object = ListView->GetItemAt(Index);
	if (!Object)
		return;

	ListView->ClearSelection();
	ListView->ScrollIndexIntoView(Index);
	ListView->SetSelectedIndex(Index);
}
