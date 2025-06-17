// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "InteractInterface.generated.h"

using FInteractionHandler = TFunction<void(int,AActor* Interactor)>;

UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTH_API IInteractInterface
{
	GENERATED_BODY()
public:

	virtual void ExecuteInteraction(AActor* Interactor) = 0;

	virtual AActor* GetInteractionActor() = 0;
	virtual FVector GetActorLocation() = 0;

	TFunction<void()> FinishInteractionCallback;
	TMap<EInteractionType, FInteractionHandler> InteractionHandlers;	
};
