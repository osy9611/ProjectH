// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonPlayerController.h"
#include "CommonLocalPlayer.h"

void ACommonPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (this->IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = this->GetLocalPlayer())
		{

			UCommonLocalPlayer* CommonLocalPlayer = Cast<UCommonLocalPlayer>(LocalPlayer);
			check(CommonLocalPlayer);

			CommonLocalPlayer->OnPlayerContollerSet.Broadcast(CommonLocalPlayer, this);
		}
	}

	OnPlayerContollerSet.Broadcast(this);
}
