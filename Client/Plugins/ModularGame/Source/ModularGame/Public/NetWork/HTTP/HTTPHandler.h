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
	int32 MaxRetry = 0;
	float RetryDelaySeconds = 0;
	bool AutoFixTrailingSlash = true;
};
USTRUCT()
struct MODULARGAME_API FPendingRequest
{
	GENERATED_BODY()
public:
	bool IsTryEnd() const { return Tried > Option.MaxRetry; }
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

	void Init(UWorld* InWorld) { World = InWorld; }
	virtual void SendGetRequest(const FString& InURL, const FHTTPRequestOption& Option, TFunction<void(const FJsonObject&, bool)> OnResponseCallback);
	virtual void SendPostRequest(const FString& InURL, const FHTTPRequestOption& Option, const TSharedPtr<FJsonObject>& JsonObject, TFunction<void(const FJsonObject&, bool)> OnResponseCallback);

	virtual bool CancelRequest(const FString& RequestId);
protected:
	void SendRequest(const FString& InVerb, const FString& InURL, const TSharedPtr<FJsonObject>& JsonObject, const FHTTPRequestOption& Option, TFunction<void(const FJsonObject&, bool)> OnResponseCallback);
	void ExecuteRequest(FPendingRequest& Req);
	virtual void OnResponseReceived(const FPendingRequest& Request, FHttpResponsePtr Response, bool bWasSuccessful);

	bool ShouldRetry(const FPendingRequest& Req, FHttpResponsePtr& Response) const;
	void ScheduleRetry(const FPendingRequest& Req);
	FString GetAuthToken();

private:
	UPROPERTY()
	TMap<FString, FPendingRequest> ActiveRequests;
	UPROPERTY()
	TWeakObjectPtr<UWorld> World;
	FHttpModule* HttpModule;
};
