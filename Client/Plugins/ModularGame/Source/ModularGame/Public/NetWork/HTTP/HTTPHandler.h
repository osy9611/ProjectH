// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HttpModule.h"
#include "HTTPHandler.generated.h"


UCLASS()
class MODULARGAME_API UHTTPHandler : public UObject
{
	GENERATED_BODY()

public:
	UHTTPHandler(const class FObjectInitializer& ObjectInitializer);

	void OnCall_Get(const FString& InURL, TFunction<void(const FJsonObject&, bool)> OnResponseCallback);
	void OnCall_Post(const FString& InURL, const TSharedPtr<FJsonObject>& JsonObject,TFunction<void(const FJsonObject&, bool)> OnResponseCallback);
private:
	void OnCall(const FString& InVerb, const FString& InURL, const TSharedPtr<FJsonObject>& JsonObject, TFunction<void(const FJsonObject&, bool)> OnResponseCallback);
	FString GetAuth();

	FHttpModule* HttpModule;
};
