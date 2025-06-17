// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonSessionSubsystem.generated.h"

UCLASS(BlueprintType)
class COMMONGAME_API UMoveSessionData : public UObject
{
	GENERATED_BODY()
public:
	FString GetMapName();

	FString CreateURL();

	UPROPERTY(BlueprintReadWrite, Category = Session)
	FPrimaryAssetId MapID;

	UPROPERTY(BlueprintReadWrite, Category = Session)
	TMap<FString, FString> ExtraArgs;
};

UCLASS()
class COMMONGAME_API UCommonSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UCommonSessionSubsystem() {}

	UFUNCTION(BlueprintCallable)
	virtual void MoveSession(APlayerController* PlayerController, UMoveSessionData* SessionData);
};
