// Fill out your copyright notice in the Description page of Project Settings.


#include "HDBattleCameraComponent.h"
#include "ProjectH/LogChannels.h"
UHDBattleCameraComponent::UHDBattleCameraComponent(const FObjectInitializer& ObjectInitializer)
{
}

AActor* UHDBattleCameraComponent::GetTargetActor() const
{
	if (!TargetActor.IsValid())
		return GetOwner();
	return TargetActor.Get();
}

void UHDBattleCameraComponent::RegisterTargetActor(AActor* Target)
{
	if (!Target)
	{
		UE_LOG(HDLog, Warning, TEXT("[BattleCameraComponent] Target is nullptr"));
		return;
	}

	TargetActor = Target;
}

void UHDBattleCameraComponent::UnregisterTargetActor()
{
	TargetActor = nullptr;
}