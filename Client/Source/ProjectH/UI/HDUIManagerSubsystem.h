// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIManagerSubsystem.h"
#include "ProjectH/UI/Container/WidgetContainer.h"
#include "ProjectH/Util/UtilFunc.h"
#include "CommonActivatableWidget.h"
#include "HDUIManagerSubsystem.generated.h"

class UHDInputConfig;

UCLASS()
class PROJECTH_API UHDUIManagerSubsystem : public UUIManagerSubsystem
{
	GENERATED_BODY()
public:
	UHDUIManagerSubsystem();

	void RegisterInputData();
	
	virtual UCommonActivatableWidget* FindWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass) final;

	template<typename T = UCommonActivatableWidget>
	T* FindWidget(EUIType Type);

public:
	UPROPERTY(EditAnywhere)
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere)
	UHDInputConfig* InputConfig;
};

template<typename T>
T* UHDUIManagerSubsystem::FindWidget(EUIType Type)
{
	FString Path = WidgetContainer::GetUIPath(Type);

	UClass* Widget = UtilFunc::LoadResource<UClass>(Path);

	return Cast<T>(FindWidget(Widget));
}
