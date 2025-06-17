// Fill out your copyright notice in the Description page of Project Settings.


#include "HDHeroComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/HDGameplayTags.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"
#include "ProjectH/Character/HDPawnData.h"
#include "ProjectH/Player/HDPlayerController.h"
#include "ProjectH/Input/HDInputConfig.h"
#include "ProjectH/Input/HDInputComponent.h"
#include "ProjectH/Input/HDInputManagerSubsystem.h"
#include "ProjectH/Interaction/InteractionComponent.h"
#include "ModularGame/Camera/ModularCameraComponent.h"
const FName UHDHeroComponent::NAME_ActorFeatureName("Hero");

UHDHeroComponent::UHDHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UHDHeroComponent::OnRegister()
{
	Super::OnRegister();

	//올바른 Actor에 등록되었는지 확인
	//OnRegister 단계에서 컴포넌트를 부착한다. 만약에 부착에 실패했다는 거는 컴포넌트 자체가 없기 때문에
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(HDLog, Error, TEXT("this component has been added to a BP whose base class is not a Pawn!!"));
			return;
		}
	}

	/*
	* GameFrameworkComponentManager에 InitState 사용을 위해 등록 진행
	*  - 등록은 상속받았던 IGameFrameworkInitStateInterface 매서드 RegisterInitStateFeature()를 활용
	* GameInstance에서 정의한 InitState 정보를 사용하기 위해서 등록하기 위한 함수로만 생각하자
	*/
	RegisterInitStateFeature();
}

void UHDHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	//PawnExtensionComponent에 대해서 (PawnExtension Feature) OnActorInitStateChanged() 관찰하도록 (Observing)
	BindOnActorInitStateChanged(UHDPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);
	ensure(TryToChangeInitState(FHDGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UHDHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UHDHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();
	if (Params.FeatureName == UHDPawnExtensionComponent::NAME_ActorFeatureName)
	{
		//PTPawnExtensionComponent의 DataInitialized 상태 변화 관찰 후, PTHeroComponent도 DataInitialized 상태로 변경
		// - CanChangInitState 확인
		if (Params.FeatureState == InitTags.InitState_DataInitialized)
			CheckDefaultInitialization();
	}
}

bool UHDHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	AHDPlayerState* HDPS = GetPlayerState<AHDPlayerState>();
	//Spawned 초기화
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
			return true;
	}

	//Spawned -> DataAvailable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!HDPS)
			return false;

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// PawnExtensionComponent가 DataInitialized될 때까지 기다림 (== 모든 Feature Component가 DataAvailable인 상태)
		return HDPS && Manager->HasFeatureReachedInitState(Pawn, UHDPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UHDHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();

	//DataAvailable -> DataInitialized단계
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AHDPlayerState* HDPS = GetPlayerState<AHDPlayerState>();
		if (!ensure(Pawn && HDPS))
			return;

		//Input과 Camera에 대한 헨들링
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UHDPawnData* PawnData = nullptr;

		if (UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UHDPawnData>();

			//DataIntialize 단계까지 오면, Pawn이 Controller에 Posses 되어 준비된 상태이다
			PawnExtComp->RegisterGameAbilitySystem(HDPS->GetHDAbilitySystemComponent(), HDPS);
		}

		if (bIsLocallyControlled && PawnData)
		{
			if (UModularCameraComponent* CameraComponent = UModularCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}

		if (AHDPlayerController* HDPC = GetController<AHDPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
				InitializePlayerInput(Pawn->InputComponent);
		}
	}
}

void UHDHeroComponent::CheckDefaultInitialization()
{
	//Pawn Extension Feature에 종속되어 있으므로, CheckDefaultInitializationForImplementers를 호출하지 않는다
	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

void UHDHeroComponent::SetAbilityCameraMode(TSubclassOf<UModularCameraMode> CameraMode, FGameplayAbilitySpecHandle& OwningSpecHandle, bool UseFovOffset)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;

		const APawn* Pawn = GetPawn<APawn>();
		if (!Pawn)
			return;
		if (UModularCameraComponent* CameraPomponent = UModularCameraComponent::FindCameraComponent(Pawn))
		{
			CameraPomponent->bAddFiledOfViewOffset = UseFovOffset;
		}
	}
}

void UHDHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle, bool UseFovOffset)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();

		const APawn* Pawn = GetPawn<APawn>();
		if (!Pawn)
			return;
		if (UModularCameraComponent* CameraPomponent = UModularCameraComponent::FindCameraComponent(Pawn))
		{
			CameraPomponent->bAddFiledOfViewOffset = UseFovOffset;
		}
	}
}

TSubclassOf<UModularCameraMode> UHDHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
		return AbilityCameraMode;

	const APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
		return nullptr;

	if (UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UHDPawnData* PawnData = PawnExtComp->GetPawnData<UHDPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return TSubclassOf<UModularCameraMode>();
}

void UHDHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
		return;

	const AActor* Actor = Cast<AActor>(Pawn);
	check(Actor);

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	// EnhancedInputLocalPlayerSubsystem 가져오기 위해
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	//UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	//check(Subsystem);

	////MappingContext 초기화
	//Subsystem->ClearAllMappings();
	//InputManagerSubsystem->RemoveAllInputContexts();

	if (const UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UHDPawnData* PawnData = PawnExtComp->GetPawnData<UHDPawnData>())
		{
			if (const UHDInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FHDGameplayTags& GameplayTags = FHDGameplayTags::Get();

				UHDInputManagerSubsystem* InputManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UHDInputManagerSubsystem>();
				if (!InputManagerSubsystem)
				{
					UE_LOG(HDLog, Error, TEXT("[HDHeroComponent] InputManagerSubsystem is nullptr"));
					return;
				}

				//Input Mapping Context를 순회하며, EnhanceInputLocalPlayerSubsystem에 추가한다
				//Enhanced Input System에서 기본 입력 매핑 리스트 중
				//자동 활성화 설정이 된 항목만 골라 플레이어 입력 서브시스템에 등록하는 구조
				//각 Config는 UInputMappingContext로 되어 있고,
				//AddPlayerMappableConfig 함수를 통해 유저가 키 리맵 가능한 매핑으로 적용된다.
				//이를 통해 초기 설정과 향후 입력 설정 변경, 저장/불러오기 기능의 기반이 됨
				InputManagerSubsystem->RegisterHDMappableConfigPairs(DefaultInputConfigs);

				//Bind GAS Action
				TArray<uint32> BindHandles;
				InputManagerSubsystem->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);

				//Bind Native Action
				InputManagerSubsystem->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
				InputManagerSubsystem->BindNativeAction(InputConfig, GameplayTags.InputTag_Interaction, ETriggerEvent::Started, this, &ThisClass::Input_Interaction, false);
				InputManagerSubsystem->BindNativeAction(InputConfig, GameplayTags.InputTag_ClickTarget, ETriggerEvent::Started, this, &ThisClass::Input_TEST, false);
			}
		}
	}
}

void UHDHeroComponent::RegisterInputAction(UHDInputConfig* InputConfig)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
		return;

}

void UHDHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	if (!bIsMove)
		return;

	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		
		Pawn->AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value.X, false);
		Pawn->AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Value.Y, false);

		if (UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnExtComp->SetFlipX(Value.X);
		}
	}
}

void UHDHeroComponent::Input_Interaction(const FInputActionValue& InputActionValue)
{
	if (!InteractionComponent)
	{
		UE_LOG(HDLog, Log, TEXT("[HDHeroComponent] Interaction Component is nullptr "));
		return;
	}
	bIsMove = false;
	InteractionComponent->StartInteraction();
}

void UHDHeroComponent::Input_TEST(const FInputActionValue& InputActionValue)
{
	UE_LOG(HDLog, Log, TEXT("[HDHeroComponent] Input_ClickTarget"));
}

void UHDHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UHDAbilitySystemComponent* ASC = PawnExtComp->GetAbilitySystemComponent())
				ASC->AbilityInputTagPressed(InputTag);
		}
	}
}

void UHDHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UHDAbilitySystemComponent* ASC = PawnExtComp->GetAbilitySystemComponent())
				ASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UHDHeroComponent::RegisterInteractionComponent(UInteractionComponent* Component)
{
	if (!Component)
		return;

	InteractionComponent = Component;
}
