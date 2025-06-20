// Fill out your copyright notice in the Description page of Project Settings.


#include "HDBattleComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/HDGameplayTags.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/Data/DataManagerSubsystem.h"
#include "ProjectH/Character/HDPawnData.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"
#include "ProjectH/Data/PlayerData/HDCharacterData.h"
#include "ProjectH/Data/PlayerData/HDPlayerDataSubsystem.h"
#include "ProjectH/AbilitySystem/Abilities/HDGameplayAbility_ActiveSkill.h"

const FName UHDBattleComponent::NAME_ActorFeatureName("Battle");

UHDBattleComponent::UHDBattleComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UHDBattleComponent::OnRegister()
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

void UHDBattleComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(UHDPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);
	ensure(TryToChangeInitState(FHDGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UHDBattleComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UHDBattleComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
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

bool UHDBattleComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	AHDPlayerState* HDPS = GetPlayerState<AHDPlayerState>();

	//Spawn 초기화
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
			return true;
	}

	//Spawned -> DataAvaliable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		//HeroComponent와 다르게 NPC는 PlayerState를 가지고 있지 않기 때문에 그냥 true로 반환 시킨다.
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

void UHDBattleComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		if (!ensure(Pawn))
			return;

		if (UHDPawnExtensionComponent* PawnExtComp = UHDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			AHDPlayerState* HDPlayerState = GetWorld()->SpawnActor<AHDPlayerState>(AHDPlayerState::StaticClass());
			if (HDPlayerState)
			{
				Pawn->SetPlayerState(HDPlayerState);
				PawnExtComp->RegisterGameAbilitySystem(HDPlayerState->GetHDAbilitySystemComponent(), HDPlayerState);
			}
		}
	}
}

void UHDBattleComponent::CheckDefaultInitialization()
{
	//Pawn Extension Feature에 종속되어 있으므로, CheckDefaultInitializationForImplementers를 호출하지 않는다
	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

void UHDBattleComponent::ProcessAbility(FGameplayTag Tag)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!ensure(Pawn))
		return;

	AHDPlayerState* HDPlayerState = Pawn->GetPlayerState<AHDPlayerState>();
	if (!HDPlayerState)
		return;

	UHDAbilitySystemComponent* ASC = HDPlayerState->GetHDAbilitySystemComponent();
	if (ASC)
		ASC->ProcessAbility(Tag);
}

void UHDBattleComponent::ProcessAbility_Skill(FGameplayTag Tag, const FBattleStateParams& Params)
{

	APawn* Pawn = GetPawn<APawn>();

	if (!ensure(Pawn))
		return;

	AHDPlayerState* HDPlayerState = Pawn->GetPlayerState<AHDPlayerState>();
	if (!HDPlayerState)
		return;

	UHDAbilitySystemComponent* ASC = HDPlayerState->GetHDAbilitySystemComponent();

	UGameplayAbility* GA = ASC->GetAbility(Tag);
	if (!GA)
		return;

	UHDGameplayAbility_ActiveSkill* GA_ActiveSkill = Cast<UHDGameplayAbility_ActiveSkill>(GA);
	if (GA_ActiveSkill)
		GA_ActiveSkill->Params = Params;

	if (ASC)
		ASC->ProcessAbility(Tag);
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void UHDBattleComponent::RegisterBattleData(UHDAttributeSet* AttributeSet)
{
	if (!AttributeSet)
		return;

	APawn* Pawn = GetPawn<APawn>();
	if (!ensure(Pawn))
		return;

	AHDPlayerState* HDPlayerState = Pawn->GetPlayerState<AHDPlayerState>();
	if (HDPlayerState)
	{
		AttributeSet->OnUpdateStatus();
		HDPlayerState->RegisterAttributeSet(AttributeSet);
	}
}

UHDAttributeSet* UHDBattleComponent::GetAttributeSet() const
{
	APawn* Pawn = GetPawn<APawn>();
	if (!ensure(Pawn))
		return nullptr;
	AHDPlayerState* HDPlayerState = Pawn->GetPlayerState<AHDPlayerState>();
	if (HDPlayerState)
	{
		return HDPlayerState->GetHDAttributeSet();
	}

	return nullptr;
}
bool UHDBattleComponent::CheckDead()
{
	UHDAttributeSet* AttributeSet = GetAttributeSet();
	if (!AttributeSet)
		return false;

	return AttributeSet->GetHP() <= 0;
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS