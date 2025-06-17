// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h" 
#include "TODHandler.generated.h"

class ATODActor;
UCLASS(Abstract, Blueprintable)
class PROJECTH_API UTODHandler : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInit(ATODActor* Actor);
	virtual void UpdateTime(float DeltaTime);

	bool IsUpdate = false;

protected:
	ATODActor* Owner;
};
