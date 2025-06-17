// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "MultiActivatableWidgetContainer.generated.h"

/**
 * 
 */
UCLASS()
class COMMONGAME_API UMultiActivatableWidgetContainer : public UCommonActivatableWidgetContainerBase
{
	GENERATED_BODY()

public:
	void AddAndActivateWidgetInstance(UCommonActivatableWidget* Widget);

	void DeactivateAllWidgets();

	const TArray<UCommonActivatableWidget*>& GetActiveWidgets() const { return  ActiveWidgets; }

protected:
	UPROPERTY()
	TArray<TObjectPtr<UCommonActivatableWidget>> ActiveWidgets;
};
