// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleGroupDefaultWidget.h"
#include "ModularGame/Log/ModularLogChannel.h"
#include "ModularGame/UI/Toggle/ToggleWidget.h"

UToggleGroupDefaultWidget::UToggleGroupDefaultWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UToggleGroupDefaultWidget::NativeConstruct()
{
}

void UToggleGroupDefaultWidget::AddToggleWidget(UToggleWidget* ToggleWidget)
{
	if (!ToggleWidget)
	{
		UE_LOG(Modular, Error, TEXT("[ToggleGroupDefaultWidget] Add Toggle Data Fail ToggleWidget is nullptr"));
		return;
	}

	ToggleWidget->OnClickEvent.AddDynamic(this, &ThisClass::OnSelectionChanged);
	Toggles.Add(ToggleWidget);
}

void UToggleGroupDefaultWidget::RemoveToggleWidget(UToggleWidget* ToggleWidget)
{
	if (!ToggleWidget)
	{
		UE_LOG(Modular, Error, TEXT("[ToggleGroupDefaultWidget] Remove Toggle Data Fail ToggleWidget is nullptr"));
		return;
	}

	if (GetToggleWiget(ToggleWidget))
	{
		ToggleWidget->OnClickEvent.RemoveDynamic(this, &ThisClass::OnSelectionChanged);
		Toggles.Remove(ToggleWidget);
	}
}

void UToggleGroupDefaultWidget::ClearToggleWidget()
{
	Toggles.Empty();
}

void UToggleGroupDefaultWidget::OnSelectionChanged(UObject* SelectedItem)
{
	UToggleWidget* SelectToggle = Cast<UToggleWidget>(SelectedItem);
	if (!SelectToggle)
	{
		UE_LOG(Modular, Error, TEXT("[ToggleGroupDefaultWidget] SelectedItem is nullptr"));
		return;
	}

	for (UToggleWidget* Toggle : Toggles)
	{
		bool bIsActive = (Toggle == SelectedItem);
		Toggle->OnActiveToggle(bIsActive);
	}
}

void UToggleGroupDefaultWidget::HandleItemClick(UObject* ClickItem)
{
}

UToggleWidget* UToggleGroupDefaultWidget::GetToggleWiget(UToggleWidget* ToggleWidget)
{
	UToggleWidget** Result = Toggles.FindByPredicate([ToggleWidget](UToggleWidget* Widget)
		{
			return Widget == ToggleWidget;
		});

	if (Result)
		return nullptr;

	return *Result;
}
