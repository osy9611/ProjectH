// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilFunc.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "ProjectH/Object/HDObjectPoolSubsystem.h"

UHDAbilitySystemComponent* UtilFunc::GetASC(AActor* Actor)
{
	UHDAbilitySystemComponent* ASC = nullptr;
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		AHDPlayerState* HDPlayerState = Pawn->GetPlayerState<AHDPlayerState>();
		ASC = HDPlayerState->GetHDAbilitySystemComponent();
	}

	return ASC;
}

UNiagaraComponent* UtilFunc::GetNiagaraSystem(UWorld* World, UNiagaraSystem* NiagaraSystem, AActor* OwnerActor, bool IsActive)
{
	if (!World)
		return nullptr;

	UHDObjectPoolSubsystem* PoolSubsystem = World->GetSubsystem<UHDObjectPoolSubsystem>();
	if (!PoolSubsystem)
	{
		UE_LOG(HDLog, Log, TEXT("[UtilFunc] HDObjectPoolSubsystem is nullptr"));
		return nullptr;
	}

	return nullptr;
}
