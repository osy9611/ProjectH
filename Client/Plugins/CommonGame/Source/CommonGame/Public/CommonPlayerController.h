// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CommonPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COMMONGAME_API ACommonPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void ReceivedPlayer() override;

	DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerControllerSetDelegate, APlayerController* PlayerController);
	FPlayerControllerSetDelegate OnPlayerContollerSet;
};
