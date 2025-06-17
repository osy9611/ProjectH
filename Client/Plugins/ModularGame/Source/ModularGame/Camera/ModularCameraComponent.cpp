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

	//EvaluateStack�� CameraModeStack�� �ִ� CameraMode�� ������Ʈ(+����)�ϰ� CameraModeStack�� Bottom-Top���� ������Ʈ�� CameraMode
	//���� Lerp�� �����Ѵ�.
	FCameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
		{
			//PlayerController�� ControlRotation�� ���� CameraModeView�� ControlRotation���� ������Ʈ
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	//CameraComponent�� ���� Parent�� SceneGraph ���踦 ����� ������Ʈ�� ����
	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);

	if (bAddFiledOfViewOffset)
		CameraModeView.FieldOfView += FieldOfViewOffset;

	if (bResetFieldOfViewOffSet)
		FieldOfViewOffset = 0.0f;

	//FOV ������Ʈ 
	FieldOfView = CameraModeView.FieldOfView;

	//FMinimalViewInfo�� ������Ʈ 
	//CameraComponent�� ��ȭ ������ �ݿ��ؼ� ���� ���������� �ݿ��ϰ� ��
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
