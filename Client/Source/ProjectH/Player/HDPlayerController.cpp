// Fill out your copyright notice in the Description page of Project Settings.


#include "HDPlayerController.h"
#include"ModularGame/Camera/ModularPlayerCameraManager.h"
#include "ProjectH/AbilitySystem/HDAbilitySystemComponent.h"
#include "ProjectH/Player/HDPlayerState.h"
AHDPlayerController::AHDPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AModularPlayerCameraManager::StaticClass();
}

void AHDPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UHDAbilitySystemComponent* ASC = GetHDAbilitySystemComponent())
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

UHDAbilitySystemComponent* AHDPlayerController::GetHDAbilitySystemComponent() const
{
	const AHDPlayerState* PS = GetPTPlayerState();
	return (PS ? PS->GetHDAbilitySystemComponent() : nullptr);
}

AHDPlayerState* AHDPlayerController::GetPTPlayerState() const
{
	return CastChecked<AHDPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}
