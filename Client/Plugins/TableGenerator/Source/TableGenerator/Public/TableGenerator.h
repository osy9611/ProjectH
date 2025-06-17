// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FTableGeneratorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void AddGenerateDataTableToolBar(FToolBarBuilder& ToolBarBuilder);
	void AddMenuBar(FMenuBarBuilder& MenuBarBuilder);

	void FillMenu(FMenuBuilder& MenuBuilder);

	void MakeDataTableStruct();
	void ImportCSVData();
	void GenerateByte();
	void SettingConfig();

	void LoadConfigData();
	void SaveConfigData();

	TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& TabSpawnArgs);

	TSharedPtr<FExtender> Extender;
	TSharedPtr<class FUICommandList> PluginCommands;

	FString CSVPathStr;
	FString EnumPathStr;
	bool bIsLiveCoding;
};
