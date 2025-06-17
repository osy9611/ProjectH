// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EUIType
{
	Battle,
};

class PROJECTH_API WidgetContainer
{
public:
	static FString GetUIPath(EUIType type);
};
