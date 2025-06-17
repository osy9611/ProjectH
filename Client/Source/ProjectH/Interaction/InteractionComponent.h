// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTH_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();
	
	void StartInteraction();

	UFUNCTION()
	void EndInteraction();
private:
	void SetInteractionActor(AActor* Actor,bool isLock);
	void CheckIntaerctionTarget(TArray<AActor*> OtherActors);
	UActorComponent* FindInteractionInterface(AActor* Actor);
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collsion", meta = (AllowPrivateAccess = "true"))
	UShapeComponent* Collision;
private:
	TWeakObjectPtr<AActor> InteractionTarget;
};
