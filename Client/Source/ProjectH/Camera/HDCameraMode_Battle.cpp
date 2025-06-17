// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCameraMode_Battle.h"

UHDCameraMode_Battle::UHDCameraMode_Battle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHDCameraMode_Battle::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = CameraRotation;

	//View ���� ���� �Ÿ� �������� �б�
	FVector CameraOffset = CameraRotation.Vector() * -CameraDistance;
	FVector FinalCameraLocation = PivotLocation + CameraOffset;


	View.Location = FinalCameraLocation;
	View.Rotation = CameraRotation;
	View.ControlRotation = CameraRotation;
	View.FieldOfView = FieldOfView;
}
