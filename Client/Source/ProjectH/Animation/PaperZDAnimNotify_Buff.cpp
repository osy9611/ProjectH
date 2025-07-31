// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperZDAnimNotify_Buff.h"
#include "PaperZDAnimInstance.h"

UPaperZDAnimNotify_Buff::UPaperZDAnimNotify_Buff()
{
}

void UPaperZDAnimNotify_Buff::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	if (!OwningInstance)
		return;

	AActor* Actor = OwningInstance->GetOwningActor();
	if (!Actor)
	{
		return;
	}

	if (OnCallback)
		OnCallback();
}
