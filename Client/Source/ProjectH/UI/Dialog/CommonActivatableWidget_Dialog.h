// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonActivatableWidget_Dialog.generated.h"

class USpeechBubbleWidget;
UCLASS()
class PROJECTH_API UCommonActivatableWidget_Dialog : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ShowDialog(int32 TableID, AActor* SpeakingActor);

	UFUNCTION(BlueprintCallable)
	void CheckNextDialog(AActor* SpeakingActor,bool& IsEnd);

	UFUNCTION(BlueprintCallable)
	void PositionSpeechBubble(AActor* SpeakingActor);
public:
	UPROPERTY(EditAnywhere)
	FVector2D BubbleOffset;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpeechBubbleWidget> SpeechBubbleWidget;
};
