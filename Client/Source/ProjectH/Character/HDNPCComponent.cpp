// Fill out your copyright notice in the Description page of Project Settings.


#include "HDNPCComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/HDGameplayTags.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/Data/EnumGenerateData.h"

const FName UHDNPCComponent::NAME_ActorFeatureName("NPC");
UHDNPCComponent::UHDNPCComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHDNPCComponent::OnRegister()
{
	Super::OnRegister();

	{
		if (GetPawn<APawn>())
		{
			UE_LOG(HDLog, Error, TEXT("this component has been added to a BP whose base class is not a Pawn!!"));
			return;
		}
	}


	RegisterInitStateFeature();
}

void UHDNPCComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(UHDPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);
	ensure(TryToChangeInitState(FHDGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UHDNPCComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UHDNPCComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
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

bool UHDNPCComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	AHDPlayerState* HDPS = GetPlayerState<AHDPlayerState>();

	//Spawn �ʱ�ȭ
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
			return true;
	}

	//Spawned -> DataAvaliable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		//HeroComponent�� �ٸ��� NPC�� PlayerState�� ������ ���� �ʱ� ������ �׳� true�� ��ȯ ��Ų��.
		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		UDataManagerSubsystem* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
		if (DataManager)
		{
			return DataManager->CheckLoadData();
		}
	
		//bool result = Manager->HasFeatureReachedInitState(Pawn, UHDPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
		return false;
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void UHDNPCComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();

	//DataAvailable -> DataInitialized �ܰ�
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		if (!ensure(Pawn))
			return;

		if (UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			Pawn->SpawnDefaultController();

			RegisterInteractionHandler();

			FNPCData* NPCData = UtilFunc_Data::GetTableData<FNPCData>(GetWorld(), FString::FromInt(TableID));
			if (NPCData)
			{
				if (OnLoadComplete.IsBound())
				{
					OnLoadComplete.Broadcast(*NPCData);
				}
			}

		}
	}
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

void UHDNPCComponent::CheckDefaultInitialization()
{
	//Pawn Extension Feature�� ���ӵǾ� �����Ƿ�, CheckDefaultInitializationForImplementers�� ȣ������ �ʴ´�
	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

void UHDNPCComponent::RegisterInteractionHandler()
{
	InteractionHandlers.Add(EInteractionType::Talk, [this](int InteractionValue, AActor* Interactor)
		{
			if (InteractionValue == 0)
			{
				FinishInteraction();
				return;
			}


			if (OnDialog.IsBound())
			{
				APawn* Pawn = GetPawn<APawn>();
				FVector MyLocation = Pawn->GetActorLocation();
				FVector TargetLocation = Interactor->GetActorLocation();

				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);

				// Yaw�� �����ؼ� ĳ���͸� �ٶ󺸰� �� (Pitch, Roll�� ���� �Ǵ� 0)
				FRotator NewRotation(0.f, LookAtRotation.Yaw, 0.f);
				Pawn->SetActorRotation(NewRotation);


				OnDialog.Broadcast(InteractionValue, Interactor);
			}
		});
}

void UHDNPCComponent::ExecuteInteraction(AActor* Interactor)
{
	if (!Interactor)
		return;

	if (FNPCData* NPCData = UtilFunc_Data::GetTableData<FNPCData>(GetWorld(), FString::FromInt(TableID)))
	{
		if (InteractionHandlers.Contains(NPCData->InteractionType))
		{
			InteractionHandlers[NPCData->InteractionType](NPCData->InteractionValue, Interactor);
		}
	}
}

void UHDNPCComponent::FinishInteraction()
{
	if (FinishInteractionCallback)
		FinishInteractionCallback();

	FinishInteractionCallback = nullptr;
}


