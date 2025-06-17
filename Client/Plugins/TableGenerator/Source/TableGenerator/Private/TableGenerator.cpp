// Copyright Epic Games, Inc. All Rights Reserved.

#include "TableGenerator.h"
#include "TableGeneratorStyle.h"
#include "TableGeneratorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include <Serialization/BufferArchive.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>


static const FName TableGeneratorTabName("TableGenerator");

#define LOCTEXT_NAMESPACE "FTableGeneratorModule"

void FTableGeneratorModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("TableGenerator Module Started"));

	//커멘드 생성
	TSharedPtr<FUICommandList> GenerateDataTableCommand = MakeShareable(new FUICommandList);
	if (!GenerateDataTableCommand)
	{
		UE_LOG(LogTemp, Error, TEXT("New FUICommandList Failed"));
		return;
	}

	FTableGeneratorCommands::Register();

	GenerateDataTableCommand->MapAction(
		FTableGeneratorCommands::Get().Cmd_MakeStruct,
		FExecuteAction::CreateRaw(this, &FTableGeneratorModule::MakeDataTableStruct),
		FCanExecuteAction()
	);

	GenerateDataTableCommand->MapAction(
		FTableGeneratorCommands::Get().Cmd_ImportCSV,
		FExecuteAction::CreateRaw(this, &FTableGeneratorModule::ImportCSVData),
		FCanExecuteAction()
	);

	GenerateDataTableCommand->MapAction(
		FTableGeneratorCommands::Get().Cmd_SettingConfig,
		FExecuteAction::CreateRaw(this, &FTableGeneratorModule::SettingConfig),
		FCanExecuteAction()
	);

	Extender = MakeShareable(new FExtender);

	Extender->AddMenuBarExtension(
		"Help",
		EExtensionHook::After,
		GenerateDataTableCommand,
		FMenuBarExtensionDelegate::CreateRaw(this, &FTableGeneratorModule::AddMenuBar)
	);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
}

void FTableGeneratorModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();

	if (Extender.IsValid())
	{
		FLevelEditorModule* LevelEditorModule = FModuleManager::GetModulePtr<FLevelEditorModule>("LevelEditor");
		if (LevelEditorModule)
		{
			LevelEditorModule->GetMenuExtensibilityManager()->RemoveExtender(Extender);
		}
		Extender.Reset();
	}

	FTableGeneratorCommands::Unregister();

	//FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TableGeneratorTabName);

}

void FTableGeneratorModule::AddGenerateDataTableToolBar(FToolBarBuilder& ToolBarBuilder)
{
	UE_LOG(LogTemp, Log, TEXT("Successfully added custom tools tool bar button"));

	FSlateIcon IconBrush = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.ViewOptions", "LevelEditor.ViewOptions.Small");

	ToolBarBuilder.AddToolBarButton(
		FTableGeneratorCommands::Get().Cmd_MakeStruct,
		NAME_None,
		FText::FromString("Custom Tools"),
		FText::FromString("This is custom tools"),
		IconBrush,
		NAME_None

	);
}

void FTableGeneratorModule::AddMenuBar(FMenuBarBuilder& MenuBarBuilder)
{
	MenuBarBuilder.AddPullDownMenu(
		FText::FromString("TableGenerator"),
		FText::FromString("Convert Excel file to csv and generated c++ struct file and data table assets."),
		FNewMenuDelegate::CreateRaw(this, &FTableGeneratorModule::FillMenu)
	);
}

void FTableGeneratorModule::FillMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("Data Table");
	{
		MenuBuilder.AddMenuEntry(
			FTableGeneratorCommands::Get().Cmd_MakeStruct,
			NAME_None,
			FText::FromString("Make Data Table"),
			FText::FromString("Make Data table"),
			FSlateIcon()
		);

		MenuBuilder.AddMenuEntry(
			FTableGeneratorCommands::Get().Cmd_ImportCSV,
			NAME_None,
			FText::FromString("Import CSV Data"),
			FText::FromString("Import CSV Data"),
			FSlateIcon()
		);

		MenuBuilder.AddMenuEntry(
			FTableGeneratorCommands::Get().Cmd_SettingConfig,
			NAME_None,
			FText::FromString("TableConfigSetting"),
			FText::FromString("Table Config Settings."),
			FSlateIcon()
		);
	}
	MenuBuilder.EndSection();
}

//외부 Python 스크립트를 실행시키는 로직
void FTableGeneratorModule::MakeDataTableStruct()
{
	UE_LOG(LogTemp, Log, TEXT(" "));
	UE_LOG(LogTemp, Log, TEXT("------------------------------"));
	UE_LOG(LogTemp, Log, TEXT("----- [TableGenerator] MakeDataTableStruct() -----"));
	UE_LOG(LogTemp, Log, TEXT(" "));

	FString PythonScriptPath = FPaths::Combine(FPaths::ProjectContentDir(), "Python", "EnumGenerator.py");
	PythonScriptPath = FPaths::ConvertRelativePathToFull(PythonScriptPath);
	PythonScriptPath.InsertAt(0, "py ");
	GEngine->Exec(nullptr, *PythonScriptPath);

	PythonScriptPath = FPaths::Combine(FPaths::ProjectContentDir(), "Python", "TableGenerator.py");
	PythonScriptPath = FPaths::ConvertRelativePathToFull(PythonScriptPath);
	PythonScriptPath.InsertAt(0, "py ");
	GEngine->Exec(nullptr, *PythonScriptPath);

	UE_LOG(LogTemp, Log, TEXT(" "));
	UE_LOG(LogTemp, Log, TEXT(" "));
	UE_LOG(LogTemp, Log, TEXT("------------------------------"));
}

void FTableGeneratorModule::ImportCSVData()
{
	UE_LOG(LogTemp, Log, TEXT(" "));
	UE_LOG(LogTemp, Log, TEXT("------------------------------"));
	UE_LOG(LogTemp, Log, TEXT("----- [TableGenerator] ImportCSVData() -----"));
	UE_LOG(LogTemp, Log, TEXT(" "));

	FString PythonScriptPath = FPaths::Combine(FPaths::ProjectContentDir(), "Python", "ImportCSV.py");
	PythonScriptPath = FPaths::ConvertRelativePathToFull(PythonScriptPath);
	PythonScriptPath.InsertAt(0, "py ");
	GEngine->Exec(nullptr, *PythonScriptPath);

	GenerateByte();

	UE_LOG(LogTemp, Log, TEXT(" "));
	UE_LOG(LogTemp, Log, TEXT(" "));
	UE_LOG(LogTemp, Log, TEXT("------------------------------"));
}

void FTableGeneratorModule::GenerateByte()
{
	FString DataTableFolderPath = FPaths::ProjectContentDir() + TEXT("Data/Data");
	TArray<FString> FileNames;
	IFileManager::Get().FindFiles(FileNames, *DataTableFolderPath, TEXT("*.uasset"));

	for (const FString& FileName : FileNames)
	{
		FString AssetPath = FString::Printf(TEXT("/Game/Data/Data/%s"), *FPaths::GetBaseFilename(FileName));
		
		if (UDataTable* DataTable = LoadObject<UDataTable>(nullptr, *AssetPath))
		{
			FString OutputPath = FPaths::ProjectContentDir()+FString::Printf(TEXT("/Data/Byte/%s.byte"), *DataTable->GetName());
			
			TArray<uint8> BinaryData;
			FMemoryWriter MemoryWriter(BinaryData, true);
			FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
			DataTable->Serialize(Ar);

			if (FFileHelper::SaveArrayToFile(BinaryData, *OutputPath))
			{
				UE_LOG(LogTemp, Log, TEXT("Data saved to binary file : %s"), *OutputPath);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Failed Data saved to binary file : %s"), *OutputPath);
			}
		}
	}
}

void FTableGeneratorModule::SettingConfig()
{
	LoadConfigData();
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		"TableGenerator",
		FOnSpawnTab::CreateRaw(this, &FTableGeneratorModule::SpawnTab))
		.SetDisplayName(LOCTEXT("TableGenerator", "TableGeneratorConfig"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
	FGlobalTabmanager::Get()->TryInvokeTab(FName("TableGenerator"));
}


void FTableGeneratorModule::LoadConfigData()
{
	FString FileStr;
	FString FilePath = FPaths::ProjectContentDir() + TEXT("/Data/Config/Config.json");

	FString JsonContent;
	if (!FFileHelper::LoadFileToString(JsonContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Json file: %s"), *FilePath);
		return;
	}

	//Json 문자열 파싱
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonContent);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		//Json 데이터 읽기
		CSVPathStr = JsonObject->GetStringField(TEXT("CSVFolderPath"));
		EnumPathStr = JsonObject->GetStringField(TEXT("EnumFolderPath"));
		bIsLiveCoding = JsonObject->GetBoolField(TEXT("UseLiveCoding"));
	}
}

void FTableGeneratorModule::SaveConfigData()
{
	if (CSVPathStr.IsEmpty())
		return;

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("CSVFolderPath", CSVPathStr);
	JsonObject->SetStringField("EnumFolderPath", EnumPathStr);
	JsonObject->SetBoolField("UseLiveCoding", bIsLiveCoding);

	FString JsonContent;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonContent);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter))
	{
		//Json 파일 저장
		FString FilePath = FPaths::ProjectContentDir() + TEXT("/Data/Config/Config.json");
		if (FFileHelper::SaveStringToFile(JsonContent, *FilePath))
		{
			UE_LOG(LogTemp, Log, TEXT("Success Json File!!"), *FilePath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Fail Json File"), *FilePath);
		}
	}
}


TSharedRef<SDockTab> FTableGeneratorModule::SpawnTab(const FSpawnTabArgs& SpawnTabArgs)
{
	TSharedRef<SDockTab> SpawendTab = SNew(SDockTab).TabRole(ETabRole::MajorTab)
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10)
				[
					SNew(SEditableTextBox)
						.HintText(LOCTEXT("InputField1Hint", "Enter first value..."))
						.Text(FText::FromString(CSVPathStr))
						.OnTextChanged_Lambda([](const FText& NewText) {
						UE_LOG(LogTemp, Log, TEXT("First Input Changed: %s"), *NewText.ToString());
							})
						.OnTextCommitted_Lambda([&](const FText& NewText, ETextCommit::Type CommitType) {
						CSVPathStr = NewText.ToString();
						UE_LOG(LogTemp, Log, TEXT("First Input Finalized: %s"), *NewText.ToString());
							})
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10)
				[
					SNew(SEditableTextBox)
						.HintText(LOCTEXT("InputField1Hint", "Enter first value..."))
						.Text(FText::FromString(EnumPathStr))
						.OnTextChanged_Lambda([](const FText& NewText) {
						UE_LOG(LogTemp, Log, TEXT("First Input Changed: %s"), *NewText.ToString());
							})
						.OnTextCommitted_Lambda([&](const FText& NewText, ETextCommit::Type CommitType) {
						EnumPathStr = NewText.ToString();
						UE_LOG(LogTemp, Log, TEXT("First Input Finalized: %s"), *NewText.ToString());
							})
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10)
				[
					SNew(SCheckBox)
						.IsChecked(bIsLiveCoding == true ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
						.OnCheckStateChanged_Lambda([&](ECheckBoxState NewState)
							{
								bIsLiveCoding = NewState == ECheckBoxState::Checked;
							})
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10)
				.HAlign(HAlign_Center) // 버튼을 중앙 정렬
				[
					SNew(SButton)
						.Text(LOCTEXT("SaveButton", "Save"))
						.OnClicked_Lambda([&]() {
						SaveConfigData();
						return FReply::Handled();
							})
				]
		];
	return SpawendTab;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTableGeneratorModule, TableGenerator)