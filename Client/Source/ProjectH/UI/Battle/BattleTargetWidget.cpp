// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTargetWidget.h"
#include "Components/SizeBox.h"
UBattleTargetWidget::UBattleTargetWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UBattleTargetWidget::SetSizeBox(const FVector& Size)
{
	TargetSizeBox->SetWidthOverride(Size.X);
	TargetSizeBox->SetHeightOverride(Size.Z);
}

void UBattleTargetWidget::OnClick()
{
	if (!OnClickCallback)
		return;


	OnClickCallback();
}
