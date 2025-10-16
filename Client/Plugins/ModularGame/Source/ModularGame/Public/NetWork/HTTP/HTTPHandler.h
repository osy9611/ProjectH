// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HttpModule.h"
#include "HTTPHandler.generated.h"

USTRUCT()
struct MODULARGAME_API FHTTPRequestOption
{
	GENERATED_BODY()
public:
	float TimeoutSecond = -1;
	int32 MaxRetry = 2;
	float RetryDelaySeconds = 2.f;
	bool AutoFixTrailingSlash = true;
};
USTRUCT()
struct MODULARGAME_API FPendingRequest
{
	GENERATED_BODY()
public:
	FString RequestId;
	FString Verb;
	FString URL;
	FString Body;
	FHTTPRequestOption Option;
	int32 Tried = 0;

	TFunction<void(const FJsonObject&, bool)> OnCallback;

	//멀티스레드를 위해서 ThreadSafe로 적용함
	//ThreadSafe를 사용하면 Spin Lock이 내부에서 돌기 때문에 안전하게 스레드 접근이 가능하지만 속도는 느림
	TSharedPtr<class IHttpRequest, ESPMode::ThreadSafe> Http;
};

UCLASS()
class MODULARGAME_API UHTTPHandler : public UObject
{
	GENERATED_BODY()

public:
	UHTTPHandler(const class FObjectInitializer& ObjectInitializer);

	void OnCall_Get(const FString& InURL, const FHTTPRequestOption& Option, TFunction<void(const FJsonObject&, bool)> OnResponseCallback);
	void OnCall_Post(const FString& InURL, const FHTTPRequestOption& Option, const TSharedPtr<FJsonObject>& JsonObject, TFunction<void(const FJsonObject&, bool)> OnResponseCallback);

	bool CancelCall(const FString& RequestId);
private:
	void OnCall(const FString& InVerb, const FString& InURL, const TSharedPtr<FJsonObject>& JsonObject, const FHTTPRequestOption& Option, TFunction<void(const FJsonObject&, bool)> OnResponseCallback);
	
	void OnResponseReceived(const FPendingRequest& Request, FHttpResponsePtr Response, bool bWasSuccessful);

	bool SouldRetry(const FPendingRequest& Req, FHttpResponsePtr& Response) const;
	void SchedulRetry(FPendingRequest Req);
	FString GetAuth();

private:
	TMap<FString, FPendingRequest> ActiveRequests;

	FHttpModule* HttpModule;

	FString EnqueueRequest(const FString& Verb, const FString& URL, const FString& Body, const FHTTPRequestOption& Option);
};
