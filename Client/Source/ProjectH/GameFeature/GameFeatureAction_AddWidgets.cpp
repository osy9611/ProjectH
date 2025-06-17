// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction_AddWidgets.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/UI/HDUIManagerSubsystem.h"
#include "CommonUserWidget.h"


void UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

void UGameFeatureAction_AddWidgets::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();

	if (!World)
	{
		UE_LOG(HDLog, Log, TEXT("[GameFeatureAction_AddWidgets] World Is nullptr"));
		return;
	}

	AddWidgets(World);
}

void UGameFeatureAction_AddWidgets::AddWidgets(UWorld* World)
{
	UHDUIManagerSubsystem* UIManager = World->GetGameInstance()->GetSubsystem<UHDUIManagerSubsystem>();
	if (!UIManager)
	{
		UE_LOG(HDLog, Error, TEXT("[GameFeatureAction_AddWidgets] UIManagerSubsystem is nullptr"));
		return;
	}
	UIManager->RemoveLayoutFromViewport();

	UCommonUserWidget* NewWidget = CreateWidget<UCommonUserWidget>(World, OverlayWidget);
	if (NewWidget)
	{
		NewWidget->AddToViewport();
	}

	if (CreateWidgets.IsEmpty())
		return;

	for (auto CreateWidget : CreateWidgets)
	{
		UIManager->CreateUI(CreateWidget.Key, CreateWidget.Value);
	}
}

void UGameFeatureAction_AddWidgets::RemoveWidgets()
{

}
