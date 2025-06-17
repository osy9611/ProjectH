// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectH/TOD/TODHandler.h"
#include "TODHandler_SunMoon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UTODHandler_SunMoon : public UTODHandler
{
	GENERATED_BODY()
public:	
	UTODHandler_SunMoon();

	virtual void OnInit(ATODActor* Actor) override;

	UFUNCTION(BlueprintCallable)
	void HandleSunMoonRotation();
	UFUNCTION(BlueprintCallable)
	void HandleVisibility();

	UPROPERTY()
	UDirectionalLightComponent* SunDirectionLight;
	UPROPERTY()
	UDirectionalLightComponent* MoonDirectionLight;
};
