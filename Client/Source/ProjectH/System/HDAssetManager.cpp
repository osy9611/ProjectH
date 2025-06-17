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

	//위의 UE_LOG의 Fatal로 인해, Crash 나기 때문에 도달하지 않지만 컴파일을 위해 더미를 리턴
	return *NewObject<UHDAssetManager>();
}
void UHDAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FHDGameplayTags::InitializeNativeTags();
}
