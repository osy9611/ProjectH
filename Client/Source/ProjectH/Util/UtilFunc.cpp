// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilFunc.h"
#include "ProjectH/Player/HDPlayerState.h"

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
