// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCameraMode_Battle.h"

UHDCameraMode_Battle::UHDCameraMode_Battle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHDCameraMode_Battle::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = CameraRotation;

	//View 카메라 뒤쪽으로 밀기
	FVector CameraOffset = CameraRotation.Vector() * -CameraDistance;
	if (UseCameraNoise)
	{
		float Time = GetWorld()->GetTimeSeconds();
		float OffsetX = FMath::PerlinNoise1D(Time * NoiseSpeed) * AmplitudeX;
		float OffsetY = FMath::PerlinNoise1D((Time + 100.0f) * NoiseSpeed) * AmplitudeY;

		CameraOffset += FVector(OffsetX, 0.0f, OffsetY);
	}

	FVector FinalCameraLocation = PivotLocation + CameraOffset;

	View.Location = FinalCameraLocation;
	View.Rotation = CameraRotation;
	View.ControlRotation = CameraRotation;
	View.FieldOfView = FieldOfView;
}
