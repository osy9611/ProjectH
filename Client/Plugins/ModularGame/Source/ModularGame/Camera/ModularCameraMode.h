// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ModularCameraMode.generated.h"

class UModularCameraComponent;

struct FCameraModeView
{
	FCameraModeView();

	void Blend(const FCameraModeView& Other, float OtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/*
* [0,1]을 BlendFunction에 맞게 재매핑을 위한 타입
*/
UENUM(BlueprintType)
enum class ECameraModeBlendFunction : uint8
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut,
	COUNT
};


UCLASS()
class MODULARGAME_API UModularCameraMode : public UObject
{
	GENERATED_BODY()
public:
	UModularCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void UpdateCameraMode(float DeltaTime);
	virtual void UpdateView(float DeltaTime);
	void UpdateBlending(float DeltaTime);

	const FCameraModeView& GetCameraModeView() const { return View; }

	UModularCameraComponent* GetCameraComponent() const;
	AActor* GetTargetActor() const;
	FVector GetPivotLocation() const;
	FVector GetPivotForward() const;
	FRotator GetPivotRotation() const;

	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float Weight);

protected:
	FCameraModeView View;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", Clampmax = "170.0"))
	float FieldOfView;

	//View에 대한 Pitch [Min, Max]
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
	float ViewPitchMax;

	//얼마동안 Blend를 진행할지 시간을 의미
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	//선형적인 Blend 값 [0, 1]
	float BlendAlpha;

	//해당 CameraMode의 최종 Blend 값
	//앞서 BlendAlpha의 선형 값을 매핑하여 최종 BlendWeight를 계산
	float BlendWeight;

	//EaseIn/Out에 사용한 Exponent
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	ECameraModeBlendFunction BlendFunction;
};

/*Camera Blending을 담당하는 객체*/
UCLASS()
class UCameraModeStack : public UObject
{
	GENERATED_BODY()
public:
	UCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UModularCameraMode* GetCameraModeInstance(TSubclassOf<UModularCameraMode>& OutCameraModeView);
	UModularCameraMode* GetFirstCameraMode() { return CameraModeInstance.Num() > 0 ? CameraModeInstance[0] : nullptr; }
	void PushCameraMode(TSubclassOf<UModularCameraMode>& CameraModeClass);
	void EvaluateStack(float DeltaTime, FCameraModeView& OutCameraModeView);
	void UpdateStack(float DeltaTime);
	void BlendStack(FCameraModeView& OutCameraModeView) const;

	//생성된 CameraMode를 관리
	UPROPERTY()
	TArray<TObjectPtr<UModularCameraMode>>CameraModeInstance;

	//Camera Matrix Blend 업데이트 진행 큐
	UPROPERTY()
	TArray<TObjectPtr<UModularCameraMode>> CameraModeStack;

};