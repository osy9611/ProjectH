// Fill out your copyright notice in the Description page of Project Settings.


#include "HDPlayerState.h"
#include <AbilitySystemComponent.h>
#include "ProjectH/Character/HDPawnData.h"
#include "ProjectH/AbilitySystem/HDAbilitySystemComponent.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/AbilitySystem/HDAbilitySet.h"
#include "ProjectH/GameModes/HDExperienceManagerComponent.h"
#include "ProjectH/GameModes/HDGameModeBase.h"

AHDPlayerState::AHDPlayerState(const FObjectInitializer& ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UHDAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
}

void AHDPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//이 부분을 안하면 AvartarActor가 등록이 안되고 Pawn 생성 시 GAS가 등록 안되는 문제가 있음
	check(AbilitySystemComponent);
	{
		//처음 InitAbilityActorInfo를 호출 당시, OwnerActor와 AvatarActor가 같은 Actor를 가르키고 있으며, 이는 PlayerState이다.
		// - OwnerActor는 PlayerState가 의도하는게 맞지만, AvatarActor는 PlayerController가 소유하는 대상인 Pawn이 되어야 한다.
		FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get();
		check(ActorInfo->OwnerActor == this);
		check(ActorInfo->OwnerActor == ActorInfo->AvatarActor);
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
	const AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);

	UHDExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UHDExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AHDPlayerState::OnExperienceLoaded(const UHDExperienceDefinition* CurrentExperience)
{
	//내 플레의 경우에는 해당 방식을 따라야하지만 AI나 NPC 같은 경우에는 직접 PawnData를 등록하는 방식으로 진행해야함
	if (AHDGameModeBase* GameModeBase = GetWorld()->GetAuthGameMode<AHDGameModeBase>())
	{
		AController* OwningController = GetOwningController();
		if (OwningController && OwningController->IsPlayerController())
		{
			//GetPawnDataForController에서 아직 PawndData를 설정하지 않았으므로, ExperienceManagerComponent의 DefautlPawnData로 설정한다.
			const UHDPawnData* NewPawnData = GameModeBase->GetPawnDataForController(GetOwningController());
			check(NewPawnData);

			SetPawnData(NewPawnData);
		}
	}
}

void AHDPlayerState::SetPawnData(const UHDPawnData* InPawnData)
{
	check(InPawnData);

	check(!PawnData);

	PawnData = InPawnData;

	RegisterAbilitySet(PawnData->AbilitySets);
}

void AHDPlayerState::RegisterAbilitySet(const TArray<TObjectPtr<UHDAbilitySet>>& AbilitySets)
{
	for (UHDAbilitySet* AbilitySet : AbilitySets)
	{
		if (AbilitySet)
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
	}
}
