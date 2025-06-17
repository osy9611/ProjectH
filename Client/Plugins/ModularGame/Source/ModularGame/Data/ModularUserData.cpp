// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularUserData.h"

void UModularUserData::RegisterData(TFunction<void(FString, bool)> UpdateContentCallBack)
{
	RegisterListener();
	
	if (UpdateContentCallBack)
		Callback = UpdateContentCallBack;
}

void UModularUserData::RegisterListener()
{
}
