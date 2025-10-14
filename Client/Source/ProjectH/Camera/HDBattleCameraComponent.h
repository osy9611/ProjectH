// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/Public/Camera/ModularCameraComponent.h"
#include "ModularGame/Public/Camera/ModularCameraMode.h"
#include "ModularGame/Public/Camera/ModularPlayerCameraManager.h"
#include "HDBattleCameraComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class PROJECTH_API UHDBattleCameraComponent : public UModularCameraComponent
{
	GENERATED_BODY()
public:
	UHDBattleCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual AActor* GetTargetActor() const override;

	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

	void RegisterTargetActor(AActor* Target);
	void UnregisterTargetActor();

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;
};
