// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCharacter.h"
#include "Components/GameFrameworkComponentManager.h"
#include "ModularGame/Public/Camera/ModularCameraComponent.h"
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
	//Actor 등록(아직 컴포넌트들이 초기화 되지 않았지만 등록만 해둠)
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AHDCharacter::BeginPlay()
{
	//GameActorReady 신호 전송
	//확장 컴포넌트들은 이벤트를 수신해 InitState 전이 처리
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

	//입력 바인딩은 PlayerController, PawnData, GAS등이 모두 초기화된 이후에 이루어져야 하기 때문이다.
	//PawnExtensionComponent는 InitState 흐름에 따라 PawnData 설정, ASC 초기화 등을 처리하며,
	//최종적으로 조건이 만족되었을 때 SetPlayerInputComponent()를 호출하여 입력 시스템을 안전하게 바인딩할 수 있기 때문
	PawnExtComponent->SetPlayerInputComponent();
}
