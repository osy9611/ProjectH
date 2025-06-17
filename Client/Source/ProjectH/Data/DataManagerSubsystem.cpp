// Fill out your copyright notice in the Description page of Project Settings.


#include "DataManagerSubsystem.h"
#include "EnumGenerateData.h"
#include "GenerateTableData.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Engine/DataTable.h"
#include "Internationalization/Culture.h"

UDataManagerSubsystem::UDataManagerSubsystem()
{

}

void UDataManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	CacheTableData.Empty();
}

void UDataManagerSubsystem::LoadAllData()
{
	if (bIsLoadData)
		return;

	UE_LOG(HDLog, Log, TEXT("[UDataManageSubSystem] LoadAllData"));
	LoadAllDataBinary();

	bIsLoadData = true;
}

void UDataManagerSubsystem::LoadAllDataTable()
{
	TMap<FString, UDataTable*> NewTableData;

	FString DataTableFolderPath = FPaths::ProjectContentDir() + TEXT("Data/Data");
	TArray<FString> FileNames;
	IFileManager::Get().FindFiles(FileNames, *DataTableFolderPath, TEXT("*.uasset"));


	for (const FString& FileName : FileNames)
	{
		FString AssetPath = FString::Printf(TEXT("/Game/Data/Data/%s"), *FPaths::GetBaseFilename(FileName));

		//이미 로드된 데이터 확인
		if (UDataTable** ExistingTable = CacheTableData.Find(FileName))
		{
			UE_LOG(HDLog, Log, TEXT("[UDataManageSubSystem] Table already loaded: %s"), *FileName);
			NewTableData.Add(FileName, *ExistingTable);
			continue;
		}

		if (UDataTable* DataTable = LoadObject<UDataTable>(nullptr, *AssetPath))
		{
			UE_LOG(HDLog, Log, TEXT("[UDataManageSubSystem] Table Load Success: %s"), *FileName);
			FString Key = *(DataTable->RowStruct->GetName());
			NewTableData.Add(Key, DataTable);
		}
		else
		{
			UE_LOG(HDLog, Error, TEXT("[UDataManageSubSystem] Table Load Fail: %s"), *FileName);
		}
	}

	//기존 데이터 덮어쓰기
	CacheTableData = MoveTemp(NewTableData);
}

void UDataManagerSubsystem::LoadAllDataBinary()
{
	if (bIsLoadData)
		return;
	TMap<FString, UDataTable*> NewTableData;

	FString DataTableFolderPath = FPaths::ProjectContentDir() + TEXT("Data/Byte");

	UE_LOG(HDLog, Error, TEXT("[UDataManageSubSystem] DataTableFolderPath : %s"), *DataTableFolderPath);

	TArray<FString> FileNames;
	IFileManager::Get().FindFiles(FileNames, *DataTableFolderPath, TEXT("*.byte"));
	UE_LOG(HDLog, Error, TEXT("[UDataManageSubSystem] FileNames Count : %i"), FileNames.Num());
	for (const FString& FileName : FileNames)
	{
		if (UDataTable* DataTable = DeserializeData(DataTableFolderPath + "/" + FileName))
		{
			FString Key = *(DataTable->RowStruct->GetName());

			UE_LOG(HDLog, Error, TEXT("[UDataManageSubSystem] BinaryData Load : %s  Key: %s"), *FileName, *Key);
			NewTableData.Add(Key, DataTable);
		}
		else
		{
			UE_LOG(HDLog, Error, TEXT("[UDataManageSubSystem] BinaryData Load Fail: %s"), *FileName);
		}
	}

	//기존 데이터 덮어쓰기
	CacheTableData = MoveTemp(NewTableData);
}

void UDataManagerSubsystem::ReloadAllData()
{
	bIsLoadData = false;
	LoadAllData();
}

UDataTable* UDataManagerSubsystem::GetTableData(const FString& TableName)
{
	if (UDataTable** TablePtr = CacheTableData.Find(TableName))
	{
		return *TablePtr;
	}
	return nullptr;
}

UDataTable* UDataManagerSubsystem::DeserializeData(const FString& FilePath)
{
	if (FilePath.IsEmpty())
	{
		UE_LOG(HDLog, Log, TEXT("[UDataManageSubSystem] This File Path Is Null"));
		return nullptr;
	}

	TArray<uint8> BinaryData;

	if (FFileHelper::LoadFileToArray(BinaryData, *FilePath) == false)
	{
		UE_LOG(HDLog, Log, TEXT("[UDataManageSubSystem] Failed to load binary file : %s"), *FilePath);
		return nullptr;
	}

	FMemoryReader MemoryReader(BinaryData, true);

	UDataTable* DataTable = NewObject<UDataTable>();

	FObjectAndNameAsStringProxyArchive Ar(MemoryReader, false);
	DataTable->Serialize(Ar);

	return DataTable;
}

FString UDataManagerSubsystem::GetCommonString(FString TableID)
{
	FString Result;
	FString Lang = FInternationalization::Get().GetCurrentLanguage()->GetName();

	FCommonString* CommonStringData = FindData<FCommonString>(TableID);
	if (!CommonStringData)
		return Result;


	if (Lang == "ko")
		Result = CommonStringData->ko;
	if (Lang == "en")
		Result = CommonStringData->en;
	if (Lang == "jp")
		Result = CommonStringData->jp;

	return Result;
}

FString UDataManagerSubsystem::GetSkillString(FString TableID)
{
	FString Result;
	FString Lang = FInternationalization::Get().GetCurrentLanguage()->GetName();

	FSkillString* SkillStringData = FindData<FSkillString>(TableID);
	if (!SkillStringData)
		return Result;


	if (Lang == "ko")
		Result = SkillStringData->ko;
	if (Lang == "en")
		Result = SkillStringData->en;
	if (Lang == "jp")
		Result = SkillStringData->jp;

	return Result;
}

FString UDataManagerSubsystem::GetDialogString(FString TableID)
{
	FString Result;
	FString Lang = FInternationalization::Get().GetCurrentLanguage()->GetName();

	FDialogData* DialogData = FindData<FDialogData>(TableID);

	if (!DialogData)
		return Result;

	if (Lang == "ko")
		Result = DialogData->ko;
	if (Lang == "en")
		Result = DialogData->en;
	if (Lang == "jp")
		Result = DialogData->jp;

	return Result;
}
