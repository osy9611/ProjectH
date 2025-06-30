// Fill out your copyright notice in the Description page of Project Settings.


#include "HDDamageWidget.h"
#include "Components/TextBlock.h"
UHDDamageWidget::UHDDamageWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UHDDamageWidget::StartDamageWidget(float DamageAmount)
{
	int32 DamageInt = static_cast<int32>(DamageAmount);
	DamageText->SetText(FText::AsNumber(DamageInt));
	SetVisibility(ESlateVisibility::Visible);
	PlayDamageWidget();
}

void UHDDamageWidget::EndDamageWidget()
{
	DamageText->SetText(FText::GetEmpty());
	SetVisibility(ESlateVisibility::Hidden);
}
