// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "HDExperienceManagerComponent.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Character/HDPawnData.h"
#include "ProjectH/GameModes/HDGameState.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/Character/HDCharacter.h"
#include "ProjectH/Character/HDNPCComponent.h"
#include "ProjectH/Player/HDPlayerController.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"
#include "ProjectH/GameModes/HDExperienceDefinition.h"
#include "ModularGame/Sound/AudioSubsystem.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/Input/HDInputManagerSubsystem.h"

AHDGameModeBase::AHDGameModeBase()
{
	GameStateClass = AHDGameState::StaticClass();
	PlayerControllerClass = AHDPlayerController::StaticClass();
	PlayerStateClass = AHDPlayerState::StaticClass();
	DefaultPawnClass = AHDCharacter::StaticClass();
}

void AHDGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	//���� GameInstance�� ����, �ʱ�ȭ �۾��� ����ǹǷ�, �� �����ӿ����� Experience ó���� ������ �� ����
	// - �̸� ó���ϱ� ����, �������� �ڿ� �̺�Ʈ�� �޾� ó���� �̾ �����Ѵ�.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExceptingOne);
}

void AHDGameModeBase::InitGameState()
{
	Super::InitGameState();

	UHDExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UHDExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* AHDGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UHDPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
			return PawnData->PawnClass;
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AHDGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

APawn* AHDGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UHDPawnData* PawnData = GetPawnDataForController(NewPlayer))
					PawnExtComp->SetPawnData(PawnData);
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}
	return nullptr;
}

AActor* AHDGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AHDGameModeBase::HandleMatchAssignmentIfNotExceptingOne()
{
	FPrimaryAssetId ExperienceId;
	FString SceneTableNo;
	UWorld* World = GetWorld();

	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		//Experience�� Value�� �����ͼ�, PrimaryAssetId�� �������ش�
		UE_LOG(HDLog, Log, TEXT("[GameModeBase] ExperienceId OptionString : %s"), *OptionsString);
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UHDExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));

		SceneTableNo = UGameplayStatics::ParseOption(OptionsString, TEXT("SceneTableNo"));
	}

	if (!ExperienceId.IsValid())
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("HDExperienceDefinition"), FName("BP_HDDefaultExperience"));

	OnMatchAssignmentGiven(ExperienceId, SceneTableNo);
}

void AHDGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, FString SceneTableNo)
{
	check(ExperienceId.IsValid());

	UHDExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UHDExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->SetCurrentExperience(ExperienceId, SceneTableNo);
}

bool AHDGameModeBase::IsExperienceLoaded() const
{
	check(GameState);
	UHDExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UHDExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

void AHDGameModeBase::OnExperienceLoaded(const UHDExperienceDefinition* CurrentExperience)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		if (PC && PC->GetPawn() == nullptr)
		{
			//RestartPlayer�� ���������� �÷��̾� ��Ʈ�ѷ��� ��������ִ°͸� �ִ°� �ƴϴ�.
			//Pawn�� ������ ���������� ���� ������ ������ ���ִ� �ͱ��� ���ش�.
			if (PlayerCanRestart(PC))
				RestartPlayer(PC);
		}
	}

	if (UAudioSubsystem* AudioSubSystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
	{
		//AudioSubSystem->RegisterData();
		//AudioSubSystem->PlaySoundAfterSceneLoading();
	}


	if (UBattleSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleSubsystem>())
	{
		BattleSubsystem->InitInput();
	}

	//NPC ������Ʈ ���� �ʱ�ȭ
	for (TActorIterator<AHDCharacter> It(GetWorld()); It; ++It)
	{
		AHDCharacter* Charecter = *It;

		if (UHDNPCComponent* NPCComp = Charecter->FindComponentByClass<UHDNPCComponent>())
		{
			// ����ġ �ε� ���� �ʱ�ȭ�� ���� Ʈ����
			NPCComp->CheckDefaultInitialization(); // or ���� Init �Լ��� ����� ȣ��
		}
	}


}

const UHDPawnData* AHDGameModeBase::GetPawnDataForController(const AController* InController) const
{
	if (InController)
	{
		if (const AHDPlayerState* PTPS = InController->GetPlayerState<AHDPlayerState>())
		{
			//GetPawnData ���� 
			if (const UHDPawnData* PawnData = PTPS->GetPawnData<UHDPawnData>())
				return PawnData;
		}
	}
	//���� PlayerState�� PawnData�� �����Ǿ� ���� ���� ���, ExperienceManagerComponent�� CurrentExperience�κ��� �����ͼ� �����Ѵ�
	check(GameState);
	UHDExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UHDExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		const UHDExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();

		ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		if (LocalPlayer)
		{
			if (LocalPlayer->PlayerController == InController)
			{
				if (Experience->DefaultPawnData)
				{
					return Experience->DefaultPawnData;
				}
			}
		}
	}

	return nullptr;
}
