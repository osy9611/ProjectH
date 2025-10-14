// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlayerDataSubsystem.generated.h"

class UModularUserData;

UCLASS(Abstract)
class MODULARGAME_API UPlayerDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void RegisterUserData() PURE_VIRTUAL(UPlayerDataSubsystem::RegisterUserData, );
	virtual void RegisterListener() PURE_VIRTUAL(UPlayerDataSubsystem::RegisterListener,);

	template<typename T= UModularUserData>
	T* GetData(FString DataType);

protected:
	UPROPERTY()
	TMap<FString, UModularUserData*> UserDatas;
};

template<typename T>
T* UPlayerDataSubsystem::GetData(FString DataType)
{
	if (!UserDatas.IsEmpty())
	{
		UModularUserData* UserData = *UserDatas.Find(DataType);
		if (UserData)
			return Cast<T>(UserData);
	}

	return nullptr;
}
