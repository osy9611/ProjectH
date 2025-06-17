// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpeechBubbleWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECTH_API USpeechBubbleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	FString GetTableString(int32 TableID);

	UFUNCTION(BlueprintCallable)
	void StartSpeech(int32 TableID);

	UFUNCTION(BlueprintCallable)
	bool FinishedRevealing();

	UFUNCTION(BlueprintCallable)
	void UpdateText();

	UFUNCTION(BlueprintCallable)
	void CheckNextSpeech(bool& IsEnd);

	UFUNCTION(BlueprintCallable)
	void SkipText();

	UFUNCTION(BlueprintImplementableEvent)
	void StartTimer();

	UFUNCTION(BlueprintImplementableEvent)
	void EndSpeech();

	UFUNCTION(BlueprintCallable)
	bool CheckDialogEnd() { return NextTableID == 0; }

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageTextDummy;


private:

	FString DisplayText;
	FString CompleteText;

	int32 CurrentTableID;
	int32 NextTableID;
};
