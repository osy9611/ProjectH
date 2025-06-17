// Fill out your copyright notice in the Description page of Project Settings.


#include "TODHandler.h"
#include "ProjectH/LogChannels.h"
void UTODHandler::OnInit(ATODActor* Actor)
{
	if (!Actor)
	{
		UE_LOG(HDLog, Log, TEXT("[TODHandler] Owter Actor is nullptr"));
		return;
	}

	Owner = Actor;
}

void UTODHandler::UpdateTime(float DeltaTime)
{
}
