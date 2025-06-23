// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/HDGameplayTags.h"
#include "ProjectH/System/HDAssetManager.h"
#include "ProjectH/Data/PlayerData/HDPlayerDataSubsystem.h"
class UHDAssetManager;
class UModularUserData;
class UHDAbilitySystemComponent;

class PROJECTH_API UtilFunc
{
public:
	template<typename T>
	static T* LoadResource(FString Path);

	template<typename T = UModularUserData>
	static T* GetUserData(UWorld* World, FString DataName);

	template<typename T = UActorComponent>
	static T* GetActorComponent(AActor* TargetActor, const FName& ComponentName);

	template<class UserClass, typename FuncType>
	static void RegisterListener(UWorld* World,FGameplayTag Channel, UserClass* Object, FuncType Func);

	template<typename StructType>
	static void RequestListener(UWorld* World, FGameplayTag Channel, StructType Message);

	static UHDAbilitySystemComponent* GetASC(AActor* Actor);
};

template<typename T>
T* UtilFunc::LoadResource(FString Path)
{
	if (Path.IsEmpty())
	{
		UE_LOG(HDLog, Log, TEXT("[UtilFunc] Resource Path is Empty"));
		return nullptr;
	}

	UHDAssetManager& AssetManager = UHDAssetManager::Get();
	UObject* Result = AssetManager.SynchronusLoadAsset(Path);
	if (!Result)
	{
		UE_LOG(HDLog, Log, TEXT("[UtilFunc] BattleResource Load Fail Can't Found Data : %s"), *Path);
		return nullptr;
	}

	return Cast<T>(Result);
}

template<typename T>
T* UtilFunc::GetUserData(UWorld* World, FString DataName)
{
	if (!World)
		return nullptr;
	UHDPlayerDataSubsystem* PlayerData = World->GetGameInstance()->GetSubsystem<UHDPlayerDataSubsystem>();

	if (!PlayerData)
	{
		UE_LOG(HDLog, Error, TEXT("[UtilFunc] HDPlayerDataSubsystem is nullptr"));
		return nullptr;
	}

	T* ResultData = PlayerData->GetData<T>(DataName);
	if (!ResultData)
	{
		UE_LOG(HDLog, Log, TEXT("[UtilFunc] %s is nullptr"), *DataName);
	}

	return ResultData;
}

template<typename T>
T* UtilFunc::GetActorComponent(AActor* TargetActor, const FName& ComponentName)
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(HDLog, Error, TEXT("[UtilFunc] TargetActor is Not Valid"));
		return nullptr;
	}

	const TSet<UActorComponent*>& Components = TargetActor->GetComponents();

	for (UActorComponent* Component : Components)
	{
		if (Component && Component->GetFName() == ComponentName)
		{
			T* TypedComponent = Cast<T>(Component);
			if (TypedComponent)
				return TypedComponent;
		}
	}

	return nullptr;
}

template<class UserClass, typename FuncType>
void UtilFunc::RegisterListener(UWorld* World,FGameplayTag Channel, UserClass* Object, FuncType Func)
{
	if (!World)
		return;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
	const FHDGameplayTags& GameplayTags = FHDGameplayTags::Get();

	MessageSubsystem.RegisterListener(Channel, Object, Func);
}

template<typename StructType>
void UtilFunc::RequestListener(UWorld* World, FGameplayTag Channel, StructType Message)
{
	if (!World)
		return;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
	MessageSubsystem.BroadcastMessage(Channel, Message);
}