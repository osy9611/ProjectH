// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "Components/ShapeComponent.h"
#include "ProjectH/Character/HDHeroComponent.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"
#include "ProjectH/Interaction/InteractInterface.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::StartInteraction()
{
	if (!Collision)
		return;

	TArray<AActor*> Actors;
	Collision->GetOverlappingActors(Actors);

	if (InteractionTarget.IsValid())
	{
		UActorComponent* ActorComponent = FindInteractionInterface(InteractionTarget.Get());
		if (!ActorComponent)
			return;

		if (IInteractInterface* Interface = Cast<IInteractInterface>(ActorComponent))
		{
			Interface->FinishInteractionCallback = [this]() { EndInteraction(); };
			Interface->ExecuteInteraction(GetOwner());
		}
	}
	else
	{
		CheckIntaerctionTarget(Actors);
	}

}

void UInteractionComponent::EndInteraction()
{
	if (InteractionTarget.IsValid())
	{
		InteractionTarget = nullptr;
		SetInteractionActor(InteractionTarget.Get(), false);
	}
	SetInteractionActor(GetOwner(), false);
	
	if (UHDHeroComponent* HeroComp = UHDHeroComponent::FindHeroComponent(GetOwner()))
	{
		HeroComp->bIsMove = true;
	}
}

void UInteractionComponent::SetInteractionActor(AActor* Actor, bool isLock)
{
	if (!Actor)
		return;

	if (UHDPawnExtensionComponent* PawnExt = UHDPawnExtensionComponent::FindPawnExtensionComponent(Actor))
	{
		PawnExt->SetInteractionLock(isLock);
	}
}

void UInteractionComponent::CheckIntaerctionTarget(TArray<AActor*> OtherActors)
{
	if (OtherActors.IsEmpty())
		return;

	for (AActor* Actor : OtherActors)
	{
		if (Actor == GetOwner())
			continue;

		UActorComponent* ActorComponent = FindInteractionInterface(Actor);
		if (ActorComponent)
		{
			InteractionTarget = Actor;
			if (IInteractInterface* Interface = Cast<IInteractInterface>(ActorComponent))
			{
				Interface->FinishInteractionCallback = [this]() { EndInteraction(); };
				Interface->ExecuteInteraction(GetOwner());
				SetInteractionActor(Actor, true);
				SetInteractionActor(GetOwner(), true);
				return;
			}
		}
	}
	
	EndInteraction();
}

UActorComponent* UInteractionComponent::FindInteractionInterface(AActor* Actor)
{
	if (!Actor)
		return nullptr;

	for (UActorComponent* Comp : Actor->GetComponents())
	{
		if (Comp && Comp->Implements<UInteractInterface>())
		{
			return Comp;
		}
	}
	return nullptr;
}
