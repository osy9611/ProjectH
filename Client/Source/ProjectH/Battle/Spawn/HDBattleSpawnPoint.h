// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "HDBattleSpawnPoint.generated.h"

UCLASS()
class PROJECTH_API AHDBattleSpawnPoint : public AActor
{
	GENERATED_BODY()
	 
public:	
	// Sets default values for this actor's properties
	AHDBattleSpawnPoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 SlotIndex;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ECharType Side;
};
