// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/Public/Camera/ModularCameraMode.h"
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

public:

	UPROPERTY(EditDefaultsOnly, Category = "Battle")
	bool UseCameraNoise = false;

	UPROPERTY(EditDefaultsOnly, Category = "Battle", Meta = (UIMin = "0.0", UIMax = "170", ClampMin = "0.0", Clampmax = "170.0"))
	float NoiseSpeed = 0.5f; //加档

	UPROPERTY(EditDefaultsOnly, Category = "Battle", Meta = (UIMin = "0.0", UIMax = "170", ClampMin = "0.0", Clampmax = "170.0"))
	float AmplitudeX = 2.0f; //如甸覆 柳气(谅快)

	UPROPERTY(EditDefaultsOnly, Category = "Battle", Meta = (UIMin = "0.0", UIMax = "170", ClampMin = "0.0", Clampmax = "170.0"))
	float AmplitudeY = 1.5f; //如甸覆 柳气(惑窍)


	UPROPERTY(EditAnywhere, Category = "Camera Rotation")
	FRotator CameraRotation;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraDistance = 3500.0f;
};
