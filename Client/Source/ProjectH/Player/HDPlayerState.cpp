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

	//�� �κ��� ���ϸ� AvartarActor�� ����� �ȵǰ� Pawn ���� �� GAS�� ��� �ȵǴ� ������ ����
	check(AbilitySystemComponent);
	{
		//ó�� InitAbilityActorInfo�� ȣ�� ���, OwnerActor�� AvatarActor�� ���� Actor�� ����Ű�� ������, �̴� PlayerState�̴�.
		// - OwnerActor�� PlayerState�� �ǵ��ϴ°� ������, AvatarActor�� PlayerController�� �����ϴ� ����� Pawn�� �Ǿ�� �Ѵ�.
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
	//�� �÷��� ��쿡�� �ش� ����� ����������� AI�� NPC ���� ��쿡�� ���� PawnData�� ����ϴ� ������� �����ؾ���
	if (AHDGameModeBase* GameModeBase = GetWorld()->GetAuthGameMode<AHDGameModeBase>())
	{
		AController* OwningController = GetOwningController();
		if (OwningController && OwningController->IsPlayerController())
		{
			//GetPawnDataForController���� ���� PawndData�� �������� �ʾ����Ƿ�, ExperienceManagerComponent�� DefautlPawnData�� �����Ѵ�.
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
