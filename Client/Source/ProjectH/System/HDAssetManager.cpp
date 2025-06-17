// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAssetManager.h"
#include "ProjectH/HDGameplayTags.h"
#include "ProjectH/LogChannels.h"
UHDAssetManager& UHDAssetManager::Get()
{
	check(GEngine);
	if (UHDAssetManager* Singletone = Cast<UHDAssetManager>(GEngine->AssetManager))
		return *Singletone;
	
	UE_LOG(HDLog, Fatal, TEXT("invalid AssetManagerClassname in DefaultEngine.ini(project setting); it must be HakAssetManager"));

	//���� UE_LOG�� Fatal�� ����, Crash ���� ������ �������� ������ �������� ���� ���̸� ����
	return *NewObject<UHDAssetManager>();
}
void UHDAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FHDGameplayTags::InitializeNativeTags();
}
