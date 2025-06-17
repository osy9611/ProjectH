// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonActivatableWidget_Dialog.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/UI/Dialog/SpeechBubbleWidget.h"
void UCommonActivatableWidget_Dialog::ShowDialog(int32 TableID, AActor* SpeakingActor)
{
	if (!SpeechBubbleWidget)
	{
		UE_LOG(HDLog, Error, TEXT("[CommonActivatableWidget_Dialog] SpeechBubble Widget is nullptr"));
		return;
	}

	SpeechBubbleWidget->StartSpeech(TableID);
	PositionSpeechBubble(SpeakingActor);
}

void UCommonActivatableWidget_Dialog::CheckNextDialog(AActor* SpeakingActor, bool& IsEnd)
{
	SpeechBubbleWidget->CheckNextSpeech(IsEnd);
	PositionSpeechBubble(SpeakingActor);
}

void UCommonActivatableWidget_Dialog::PositionSpeechBubble(AActor* SpeakingActor)
{
	if (!SpeakingActor)
	{
		UE_LOG(HDLog, Error, TEXT("[CommonActivatableWidget_Dialog] SpeakingActor is nullptr"));
		return;
	}

	FVector WorldLocation = SpeakingActor->GetActorLocation();
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	FVector2D ScreenLoc;

	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, WorldLocation, ScreenLoc, false);

	this->ForceLayoutPrepass();
	SpeechBubbleWidget->ForceLayoutPrepass();

	FVector2D DesiredSize = SpeechBubbleWidget->GetDesiredSize();
	ScreenLoc -= FVector2D(DesiredSize.X * 0.5f, DesiredSize.Y);	//중앙 하단 정렬
	ScreenLoc -= BubbleOffset;

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(SpeechBubbleWidget->Slot))
	{
		CanvasSlot->SetPosition(ScreenLoc);
	}
}