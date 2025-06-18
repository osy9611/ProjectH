// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleInput.h"
#include "PlayerMappableInputConfig.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/HDGameplayTags.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Input/HDInputManagerSubsystem.h"
#include "ProjectH/Input/Container/HDInputContainer.h"
#include "ProjectH/Input/HDInputConfig.h"

UBattleInput::UBattleInput(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UBattleInput::OnInit()
{
	UHDInputManagerSubsystem* InputManager = GetWorld()->GetGameInstance()->GetSubsystem<UHDInputManagerSubsystem>();
	if (!InputManager)
	{
		UE_LOG(HDLog, Error, TEXT("[BattleInput] HDInputManagerSubsystem is nullptr"));
		return;
	}

	//Load Mapping Context
	InputManager->LoadInputContext(EInputMappingType::Battle, true);

	//Load Input Config
	FString InputConfigPath = HDInputContainer::GetHDInputConfigPath(EInputMappingType::Battle);
	UHDInputConfig* Result = UtilFunc::LoadResource<UHDInputConfig>(InputConfigPath);

	if (!Result)
	{
		UE_LOG(HDLog, Log, TEXT("[BattleInput] HDInputConfig Load Fail Path : %s"), *InputConfigPath);
		return;
	}

	InputConfig = Result;

	const FHDGameplayTags& GameplayTags = FHDGameplayTags::Get();
}
