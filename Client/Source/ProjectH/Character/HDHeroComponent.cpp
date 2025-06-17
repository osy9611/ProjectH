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

	//�ùٸ� Actor�� ��ϵǾ����� Ȯ��
	//OnRegister �ܰ迡�� ������Ʈ�� �����Ѵ�. ���࿡ ������ �����ߴٴ� �Ŵ� ������Ʈ ��ü�� ���� ������
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(HDLog, Error, TEXT("this component has been added to a BP whose base class is not a Pawn!!"));
			return;
		}
	}

	/*
	* GameFrameworkComponentManager�� InitState ����� ���� ��� ����
	*  - ����� ��ӹ޾Ҵ� IGameFrameworkInitStateInterface �ż��� RegisterInitStateFeature()�� Ȱ��
	* GameInstance���� ������ InitState ������ ����ϱ� ���ؼ� ����ϱ� ���� �Լ��θ� ��������
	*/
	RegisterInitStateFeature();
}

void UHDHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	//PawnExtensionComponent�� ���ؼ� (PawnExtension Feature) OnActorInitStateChanged() �����ϵ��� (Observing)
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
		//PTPawnExtensionComponent�� DataInitialized ���� ��ȭ ���� ��, PTHeroComponent�� DataInitialized ���·� ����
		// - CanChangInitState Ȯ��
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
	//Spawned �ʱ�ȭ
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
		// PawnExtensionComponent�� DataInitialized�� ������ ��ٸ� (== ��� Feature Component�� DataAvailable�� ����)
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

	//DataAvailable -> DataInitialized�ܰ�
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AHDPlayerState* HDPS = GetPlayerState<AHDPlayerState>();
		if (!ensure(Pawn && HDPS))
			return;

		//Input�� Camera�� ���� ��鸵
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UHDPawnData* PawnData = nullptr;

		if (UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UHDPawnData>();

			//DataIntialize �ܰ���� ����, Pawn�� Controller�� Posses �Ǿ� �غ�� �����̴�
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
	//Pawn Extension Feature�� ���ӵǾ� �����Ƿ�, CheckDefaultInitializationForImplementers�� ȣ������ �ʴ´�
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

	// EnhancedInputLocalPlayerSubsystem �������� ����
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	//UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	//check(Subsystem);

	////MappingContext �ʱ�ȭ
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

				//Input Mapping Context�� ��ȸ�ϸ�, EnhanceInputLocalPlayerSubsystem�� �߰��Ѵ�
				//Enhanced Input System���� �⺻ �Է� ���� ����Ʈ ��
				//�ڵ� Ȱ��ȭ ������ �� �׸� ��� �÷��̾� �Է� ����ý��ۿ� ����ϴ� ����
				//�� Config�� UInputMappingContext�� �Ǿ� �ְ�,
				//AddPlayerMappableConfig �Լ��� ���� ������ Ű ���� ������ �������� ����ȴ�.
				//�̸� ���� �ʱ� ������ ���� �Է� ���� ����, ����/�ҷ����� ����� ����� ��
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
