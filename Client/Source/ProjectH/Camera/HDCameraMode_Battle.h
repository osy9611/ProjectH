// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/Camera/ModularCameraMode.h"
#include "HDCameraMode_Battle.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class PROJECTH_API UHDCameraMode_Battle : public UModularCameraMode
{
	GENERATED_BODY()
public:
	UHDCameraMode_Battle(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void UpdateView(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Camera Rotation")
	FRotator CameraRotation;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraDistance = 3500.0f;
};
