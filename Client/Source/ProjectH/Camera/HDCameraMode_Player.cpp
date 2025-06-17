// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCameraMode_Player.h"
#include "Curves/CurveVector.h"
#include "ProjectH/LogChannels.h"
#include "ModularGame/Camera/ModularCameraComponent.h"

UHDCameraMode_Player::UHDCameraMode_Player(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHDCameraMode_Player::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = CameraRotation;

	//View 방향 기준 거리 뒤쪽으로 밀기
	FVector CameraOffset = CameraRotation.Vector() * -CameraDistance;
	FVector FinalCameraLocation = PivotLocation + CameraOffset;


	View.Location = FinalCameraLocation;
	View.Rotation = CameraRotation;
	View.ControlRotation = CameraRotation;
	View.FieldOfView = FieldOfView;
}
