// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperZDAnimNotify_Damage.h"
#include "PaperZDAnimInstance.h"
#include "ProjectH/LogChannels.h"
UPaperZDAnimNotify_Damage::UPaperZDAnimNotify_Damage()
{
}

void UPaperZDAnimNotify_Damage::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
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
