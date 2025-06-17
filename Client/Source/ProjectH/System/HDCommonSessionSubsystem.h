// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonSessionSubsystem.h"
#include "HDCommonSessionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UHDCommonSessionSubsystem : public UCommonSessionSubsystem
{
	GENERATED_BODY()
public:
	UHDCommonSessionSubsystem();

	UFUNCTION(BlueprintCallable)
	void MoveSeesionByTableID(int32 TableNo);
};
