// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "ModularGame/Public/Network/HTTP/HTTPHandler.h"
#include "ModularGame/Public/Log/ModularLogChannel.h"
#include "Runtime/Online/HTTP/Public/Http.h"

UHTTPHandler::UHTTPHandler(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HttpModule = &FHttpModule::Get();
}

void UHTTPHandler::SendGetRequest(const FString& InURL, const FHTTPRequestOption& Option, TFunction<void(const FJsonObject&, bool)> OnResponseCallback)
{
	if (InURL.IsEmpty())
	{
		UE_LOG(Modular, Error, TEXT("[HTTPHandler] Get Fail URL is Empty"));
		return;
	}

	if (!OnResponseCallback)
	{
		UE_LOG(Modular, Error, TEXT("[HTTPHandler] Get Fail OnResponseCallback is null"));
		return;
	}

	SendRequest("GET", InURL, nullptr, Option, OnResponseCallback);
}

void UHTTPHandler::SendPostRequest(const FString& InURL, const FHTTPRequestOption& Option, const TSharedPtr<FJsonObject>& JsonObject, TFunction<void(const FJsonObject&, bool)> OnResponseCallback)
{
	if (InURL.IsEmpty())
	{
		UE_LOG(Modular, Error, TEXT("[HTTPHandler] Post Fail URL is Empty"));
		return;
	}

	if (!OnResponseCallback)
	{
		UE_LOG(Modular, Error, TEXT("[HTTPHandler] Post Fail OnResponseCallback is null"));
		return;
	}

	SendRequest("POST", InURL, JsonObject, Option, OnResponseCallback);
}

bool UHTTPHandler::CancelRequest(const FString& RequestId)
{
	if (FPendingRequest* Request = ActiveRequests.Find(RequestId))
	{
		Request->Http->CancelRequest();
		ActiveRequests.Remove(RequestId);
		UE_LOG(Modular, Log, TEXT("[HTTPHandler] Cancelled Request: %s"), *RequestId);
		return true;
	}

	return false;
}

void UHTTPHandler::SendRequest(const FString& InVerb, const FString& InURL, const TSharedPtr<FJsonObject>& JsonObject, const FHTTPRequestOption& Option, TFunction<void(const FJsonObject&, bool)> OnResponseCallback)
{
	FPendingRequest Req;
	Req.RequestId = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
	Req.Verb = InVerb;
	Req.URL = InURL;
	Req.Option = Option;
	Req.OnCallback = MoveTemp(OnResponseCallback);

	FString RequestBody;
	if (JsonObject)
	{
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	}
	Req.Body = RequestBody;

	ExecuteRequest(Req);
}

void UHTTPHandler::ExecuteRequest(FPendingRequest& Req)
{
	Req.Http = HttpModule->CreateRequest();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Req.Http.ToSharedRef();
	Request->OnProcessRequestComplete().BindLambda([this, RequestId = Req.RequestId](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (FPendingRequest* Found = ActiveRequests.Find(RequestId))
			{
				OnResponseReceived(*Found, Response, bWasSuccessful);
			}
		});

	Request->SetURL(Req.URL);
	Request->SetVerb(Req.Verb);

	Request->SetHeader("Content-Type", TEXT("application/json"));
	//TODO : 현재는 Auth가 어떻게 들어올지 모르기 때문에 임의로 작업을 진행한다.
	FString Auth = GetAuthToken();
	if (Auth.IsEmpty())
		Request->SetHeader("Authorization", FString::Printf(TEXT("Basic %s"), *Auth));

	//Set HTTP TimeOut
	if (Req.Option.TimeoutSecond > -1)
		Request->SetTimeout(Req.Option.TimeoutSecond);


	Request->SetContentAsString(Req.Body);
	Request->ProcessRequest();

	Req.Tried++;
	if (!ActiveRequests.Contains(Req.RequestId))
		ActiveRequests.Add(Req.RequestId, Req);
}

void UHTTPHandler::OnResponseReceived(const FPendingRequest& Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(Modular, Error, TEXT("[HTTPHandler] RequestFailed URL = %s "), *Request.URL);
		Request.OnCallback(FJsonObject(), false);
		ActiveRequests.Remove(Request.RequestId);
		return;
	}

	int32 ResponseCode = Response->GetResponseCode();

	//Check Response Code
	if (ResponseCode >= 200 && ResponseCode<300)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			Request.OnCallback(*JsonObject, true);
			ActiveRequests.Remove(Request.RequestId);
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("HTTP Error (%d): %s"), ResponseCode, *Response->GetContentAsString());

		if (ShouldRetry(Request, Response))
		{
			//ActiveRequests.Remove(Request.RequestId);
			ScheduleRetry(Request);
			return;
		}
	}

	ActiveRequests.Remove(Request.RequestId);
}

bool UHTTPHandler::ShouldRetry(const FPendingRequest& Req, FHttpResponsePtr& Response) const
{
	if (Req.Option.MaxRetry <= 0)
		return false;

	if (Req.IsTryEnd())
		return false;

	int32 ResponseCode = Response->GetResponseCode();
	//Error Code 500 
	if (ResponseCode >= 500 && ResponseCode <= 599)
		return true;
	//Too Many Requests
	if (ResponseCode == 429)
		return true;

	return false;
}

void UHTTPHandler::ScheduleRetry(const FPendingRequest& Req)
{
	if (!World.IsValid())
		return;

	const float Delay = FMath::Max(0, Req.Option.RetryDelaySeconds);

	if (Req.IsTryEnd())
	{
		UE_LOG(Modular, Warning, TEXT("[HTTPHandler] Retry Limit : %s"), *Req.URL);
		return;
	}

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this, Id = Req.RequestId]()
			{
				if (FPendingRequest* Found = ActiveRequests.Find(Id))
				{
					if (Found->Http.IsValid())
					{
						Found->Http->CancelRequest();
						Found->Http.Reset();
					}

					ExecuteRequest(*Found);
				}
			}),
		Req.Option.RetryDelaySeconds, false
	);
}

//TODO : 추후 어떻게 Auth가 들어올지 모르기 때문에 우선적으로 러프하게 작업을 진행한다.
FString UHTTPHandler::GetAuthToken()
{

	return FString();
}
