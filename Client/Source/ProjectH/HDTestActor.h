// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDTestActor.generated.h"

UENUM(BlueprintType)
enum class EEffectType : uint8
{
	None        UMETA(DisplayName = "None"),
	Fire        UMETA(DisplayName = "Fire"),
	Ice         UMETA(DisplayName = "Ice")
};

UCLASS()
class PROJECTH_API AHDTestActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHDTestActor();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectType EffectType = EEffectType::None;

	UPROPERTY()
	bool bEnableFirePower = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bEnableFirePower"))
	float FirePower = 100.0f;

	UPROPERTY()
	bool bEnableIcePower = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bEnableIcePower"))
	float IcePower = 50.0f;
};
