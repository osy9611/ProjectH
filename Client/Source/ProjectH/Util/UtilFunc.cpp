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

TArray<FText> UtilFunc::EnumToText(const UEnum* EnumPtr,int8 Max)
{
	TArray<FText> Result;

	for (int32 i = 0; i < Max; ++i)
	{
		if (!EnumPtr->IsValidEnumValue(i))
			continue;

		FText Name = EnumPtr->GetDisplayNameTextByValue(i);
		Result.Add(Name);
	}

	return Result;
}

FName UtilFunc::EnumToName(const UEnum* EnumPtr, int8 Type)
{
	if (EnumPtr)
	{
		FString NameString = EnumPtr->GetNameStringByValue(Type);
		return FName(NameString);
	}

	return FName();
}


