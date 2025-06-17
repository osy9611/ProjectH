// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharActiveSelect.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Util/UtilFunc_Data.h"
UBattleCharActiveSelect::UBattleCharActiveSelect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FReply UBattleCharActiveSelect::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnClick();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void UBattleCharActiveSelect::OnActiveToggle(bool IsActive)
{
	Super::OnActiveToggle(IsActive);

	if (IsActive)
	{
		Image->SetOpacity(1.0f);
	}
	else
	{
		Image->SetOpacity(0.5f);
	}
}

void UBattleCharActiveSelect::OnInit(FString ToggleText)
{
	FString SkillName = UtilFunc_Data::GetSkillString(GetWorld(), ToggleText);

	Text->SetText(FText::FromString(SkillName));
}
