// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularCameraComponent.h"
#include "ModularGame/Camera/ModularCameraMode.h"

UModularCameraComponent::UModularCameraComponent(const FObjectInitializer& ObjectInitializer)
{
	FieldOfViewOffset = 0;
}

void UModularCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	if (DetermineCameraModeDelegate.IsBound())
	{
		if (TSubclassOf<UModularCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
		{
			CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}

void UModularCameraComponent::OnRegister()
{
	Super::OnRegister();
	if (!CameraModeStack)
		CameraModeStack = NewObject<UCameraModeStack>(this);
}

void UModularCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	check(CameraModeStack);

	UpdateCameraModes();

	//EvaluateStack은 CameraModeStack에 있는 CameraMode를 업데이트(+블랜딩)하고 CameraModeStack을 Bottom-Top까지 업데이트된 CameraMode
	//들을 Lerp를 진행한다.
	FCameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
		{
			//PlayerController의 ControlRotation을 계산된 CameraModeView의 ControlRotation으로 업데이트
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	//CameraComponent에 대해 Parent의 SceneGraph 관계를 고려해 업데이트를 진행
	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);

	if (bAddFiledOfViewOffset)
		CameraModeView.FieldOfView += FieldOfViewOffset;

	if (bResetFieldOfViewOffSet)
		FieldOfViewOffset = 0.0f;

	//FOV 업데이트 
	FieldOfView = CameraModeView.FieldOfView;

	//FMinimalViewInfo를 업데이트 
	//CameraComponent의 변화 사항을 반영해서 최종 렌더링까지 반영하게 됨
	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)
		DesiredView.PostProcessSettings = PostProcessSettings;
}
