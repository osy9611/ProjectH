// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TableGeneratorStyle.h"

class FTableGeneratorCommands : public TCommands<FTableGeneratorCommands>
{
public:

	FTableGeneratorCommands()
		: TCommands<FTableGeneratorCommands>(TEXT("TableGenerator"), NSLOCTEXT("Contexts", "TableGenerator", "TableGenerator Plugin"), NAME_None, FTableGeneratorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	TSharedPtr< FUICommandInfo > Cmd_MakeStruct;
	TSharedPtr< FUICommandInfo > Cmd_ImportCSV;
	TSharedPtr< FUICommandInfo > Cmd_SettingConfig;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};