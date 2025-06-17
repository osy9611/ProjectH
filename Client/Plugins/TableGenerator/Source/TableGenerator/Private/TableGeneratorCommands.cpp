// Copyright Epic Games, Inc. All Rights Reserved.

#include "TableGeneratorCommands.h"

#define LOCTEXT_NAMESPACE "FTableGeneratorModule"

void FTableGeneratorCommands::RegisterCommands()
{
	UI_COMMAND(Cmd_MakeStruct, "Make data table struct", "Make C++ sturct", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(Cmd_ImportCSV, "Make data table asset", "Make data table asset", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(Cmd_SettingConfig, "Setting Config", "Setting Config", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
