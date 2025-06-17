// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "ModularCameraMode.h"
#include "ModularCameraComponent.generated.h"

class UCameraModeStack;

template<class TClass> class TSubclassOf;

DECLARE_DELEGATE_RetVal(TSubclassOf<UModularCameraMode>, FCameraModeDelegate);

UCLASS(Blueprintable, ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class MODULARGAME_API UModularCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
public:
	UModularCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AActor* GetTargetActor()const { return GetOwner(); }
	void UpdateCameraModes();

	static UModularCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UModularCameraComponent>() : nullptr); }

	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

	//카메라의 Blending 기능을 지원하는 Stack
	UPROPERTY()
	TObjectPtr<UCameraModeStack> CameraModeStack;

	//현재 CameraMode를 가져오는 Delegate
	FCameraModeDelegate DetermineCameraModeDelegate;

	void AddFieldOfViewOffset(float Value, bool ResetFieldOfViewOffSet)
	{
		FieldOfViewOffset = FMath::Clamp(FieldOfViewOffset - Value * 100.0f * 0.016f, 0.0f, 20.0f);
		bResetFieldOfViewOffSet = ResetFieldOfViewOffSet;
	}

public:
	bool bResetFieldOfViewOffSet;
	bool bAddFiledOfViewOffset;
protected:

	float FieldOfViewOffset;
};
