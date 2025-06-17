// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharActiveWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/HDGameplayTags.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/UI/Battle/BattleCharActiveSelect.h"

UBattleCharActiveWidget::UBattleCharActiveWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UBattleCharActiveWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ActiveTags.IsEmpty())
	{
		const FHDGameplayTags& GameplayTags = FHDGameplayTags::Get();
		ActiveTags = { GameplayTags.Battle_Skill1, GameplayTags.Battle_Skill2,GameplayTags.Battle_Skill3,GameplayTags.Battle_Skill4 };
	}

	GetToggles();
}

void UBattleCharActiveWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
}

void UBattleCharActiveWidget::OnSelectionChanged(UObject* SelectedItem)
{
	Super::OnSelectionChanged(SelectedItem);

}

void UBattleCharActiveWidget::GetToggles()
{
	if (!ToggleRoot)
		return;

	int32 ChildCount = ToggleRoot->GetChildrenCount();

	for (int32 i = 0; i < ChildCount; ++i)
	{
		UWidget* Widget = ToggleRoot->GetChildAt(i);
		if (!Widget)
			continue;

		UBattleCharActiveSelect* SelectWidget = Cast<UBattleCharActiveSelect>(Widget);
		if (!SelectWidget)
			continue;

		SelectWidget->ActiveTag = ActiveTags[i];
		SelectWidget->Image->SetOpacity(0.5f);
		AddToggleWidget(SelectWidget);
	}
}

void UBattleCharActiveWidget::UpdateToggles(const FCharacterData& CharacterData)
{
	int32 Index = 0;
	for (const FString& SkillName : CharacterData.SkillNames)
	{
		UToggleWidget* ToggleWidget = Toggles[Index];
		if (!ToggleWidget)
			continue;

		UBattleCharActiveSelect* SelectWidget = Cast<UBattleCharActiveSelect>(ToggleWidget);
		if (!SelectWidget)
			continue;

		SelectWidget->OnInit(SkillName);

		Index++;
	}
}
