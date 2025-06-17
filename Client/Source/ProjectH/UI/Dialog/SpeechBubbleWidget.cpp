// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeechBubbleWidget.h"
#include "Components/TextBlock.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "ProjectH/Util/UtilFunc_Data.h"
FString USpeechBubbleWidget::GetTableString(int32 TableID)
{
	UDataManagerSubsystem* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
	if(!DataManager)
	{
		UE_LOG(HDLog,Error,TEXT("[SpeechBubbleWidget] DataManager is nullptr"));
		return FString();
	}

	return DataManager->GetDialogString(FString::FromInt(TableID));
}

void USpeechBubbleWidget::StartSpeech(int32 TableID)
{
	FString TableString = GetTableString(TableID);
	if (TableString.IsEmpty())
		return;

	FDialogData* DialogData = UtilFunc_Data::GetTableData<FDialogData>(GetWorld(), FString::FromInt(TableID));
	if (!DialogData)
	{
		UE_LOG(HDLog, Log, TEXT("[SpeechBubbleWidget] Dialog Data Load Fail TableID : %s Not Found"), *FString::FromInt(TableID));
		return;
	}

	//Set Name Text
	FString SpeakerName = UtilFunc_Data::GetCommonString(GetWorld(), DialogData->Speaker);
	NameText->SetText(FText::FromString(SpeakerName));

	//Set Complete Text
	CompleteText = TableString;
	DisplayText = nullptr;
	MessageTextDummy->SetText(FText::FromString(CompleteText));


	//Update Current Table ID
	CurrentTableID = TableID;
	NextTableID = DialogData->NextDailogNo;

	StartTimer();
}

bool USpeechBubbleWidget::FinishedRevealing()
{
	return DisplayText.Len() >= CompleteText.Len();
}

void USpeechBubbleWidget::UpdateText()
{	
	DisplayText = CompleteText.Mid(0, DisplayText.Len() + 1);
	MessageText->SetText(FText::FromString(DisplayText));
}

void USpeechBubbleWidget::CheckNextSpeech(bool& IsEnd)
{
	if (NextTableID == 0)
	{
		EndSpeech();
		IsEnd = true;
		return;
	}
	IsEnd = false;
	StartSpeech(NextTableID);
}

void USpeechBubbleWidget::SkipText()
{
	DisplayText = CompleteText;
	MessageText->SetText(FText::FromString(DisplayText));
}