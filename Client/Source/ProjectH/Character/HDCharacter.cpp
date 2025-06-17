// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCharacter.h"
#include "Components/GameFrameworkComponentManager.h"
#include "ModularGame/Camera/ModularCameraComponent.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"

AHDCharacter::AHDCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PawnExtComponent = CreateDefaultSubobject<UHDPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
}

void AHDCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AHDCharacter::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void AHDCharacter::EndPlay(const EEndPlayReason::Type EndPlayReson)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReson);
}

void AHDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PawnExtComponent->SetPlayerInputComponent();
}
