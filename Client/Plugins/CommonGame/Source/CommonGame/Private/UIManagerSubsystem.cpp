// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManagerSubsystem.h"
#include "CommonLocalPlayer.h"
#include "CommonPlayerController.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"


UUIManagerSubsystem::UUIManagerSubsystem()
{
}

void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UUIManagerSubsystem::NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer)
{
	check(LocalPlayer);

	LocalPlayer->OnPlayerContollerSet.AddWeakLambda(this, [this](UCommonLocalPlayer* LocalPlayer, APlayerController* PlayerController)
		{
			if (LocalPlayer)
			{
				ACommonPlayerController* CommonPlayerController = Cast<ACommonPlayerController>(PlayerController);

				NotifyPlayerRemoved(LocalPlayer);
			}
		});
}

void UUIManagerSubsystem::NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer)
{
	RemoveLayoutFromViewport();
}

void UUIManagerSubsystem::NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer)
{
	check(LocalPlayer);

	LocalPlayer->OnPlayerContollerSet.RemoveAll(this);
	RemoveLayoutFromViewport();
}

void UUIManagerSubsystem::RemoveLayoutFromViewport()
{
	for (auto& Layer : Layers)
	{
		UCommonActivatableWidgetContainerBase* Container = Layer.Value;

		if (!IsValid(Container))
			continue;

		const TArray<UCommonActivatableWidget*>& WidgetList = Container->GetWidgetList();

		if (!WidgetList.IsEmpty())
		{
			TArray<UCommonActivatableWidget*> CopyList = WidgetList;
			for (UCommonActivatableWidget* Widget : CopyList)
			{
				if (IsValid(Widget))
				{
					//비활성화 및 제거
					Widget->DeactivateWidget();
					Widget->RemoveFromParent();
				}
			}
		}

		Container->ClearWidgets();
		Container->RemoveFromParent();
	}

	Layers.Empty();
	CacheWidgets.Empty();
}

void UUIManagerSubsystem::RegisterWidgetContainer(FGameplayTag LayerName, UCommonActivatableWidgetContainerBase* Container)
{
	if (UCommonActivatableWidgetContainerBase* LayerWidget = GetLayerContainer(LayerName))
	{
		return;
	}

	Container->SetTransitionDuration(0.0f);
	Layers.Add(LayerName, Container);
}

UCommonActivatableWidget* UUIManagerSubsystem::CreateUI(FGameplayTag LayerTag, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (UCommonActivatableWidgetContainerBase* LayerWidget = GetLayerContainer(LayerTag))
	{
		if (UCommonActivatableWidget* Widget = CacheWidgets.FindRef(WidgetClass))
		{
			return Widget;
		}

		UWorld* World = GetWorld();
		if (!World)
		{
			UE_LOG(LogTemp, Error, TEXT("[UIManagerSubsystem] World is nullptr"));
			return nullptr;
		}

		UCommonActivatableWidget* NewWidget = CreateWidget<UCommonActivatableWidget>(World, WidgetClass);
		NewWidget->AddToViewport();
		LayerWidget->AddWidgetInstance(*NewWidget);
		CacheWidgets.Add(WidgetClass, NewWidget);

		return NewWidget;
	}

	return nullptr;
}

UCommonActivatableWidget* UUIManagerSubsystem::FindWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (UCommonActivatableWidget* Widget = CacheWidgets.FindRef(WidgetClass))
	{
		return Widget;
	}
	return nullptr;
}
