// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularGame/Public/UI/Toggle/ToggleWidget.h"

UToggleWidget::UToggleWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UToggleWidget::OnActiveToggle(bool IsActive)
{

}

void UToggleWidget::OnClick()
{
	if (OnClickEvent.IsBound())
	{
		OnClickEvent.Broadcast(this);
	}
}
