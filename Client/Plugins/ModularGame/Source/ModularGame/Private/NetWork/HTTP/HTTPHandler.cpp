// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularGame/Public/Network/HTTP/HTTPHandler.h"
#include "ModularGame/Public/Log/ModularLogChannel.h"
#include "Runtime/Online/HTTP/Public/Http.h"

UHTTPHandler::UHTTPHandler(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HttpModule = &FHttpModule::Get();
}

void UHTTPHandler::OnCall_Get(const FString& InURL, const FHTTPRequestOption& Option, TFunction<void(const FJsonObject&, bool)> OnResponseCallback)
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

	OnCall("GET", InURL, nullptr, Option, OnResponseCallback);
}

void UHTTPHandler::OnCall_Post(const FString& InURL, const FHTTPRequestOption& Option, const TSharedPtr<FJsonObject>& JsonObject, TFunction<void(const FJsonObject&, bool)> OnResponseCallback)
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

	OnCall("POST", InURL, JsonObject, Option, OnResponseCallback);
}

bool UHTTPHandler::CancelCall(const FString& RequestId)
{
	if (FPendingRequest* Request = ActiveRequests.Find(RequestId))
	{
		Request->Http->CancelRequest();
		ActiveRequests.Remove(RequestId);
		//UE_LOG(Modular, Log, TEXT("[HTTPHandler] Cancelled Request: %d"), *RequestId);
		return true;
	}

	return false;
}

void UHTTPHandler::OnCall(const FString& InVerb, const FString& InURL, const TSharedPtr<FJsonObject>& JsonObject, const FHTTPRequestOption& Option, TFunction<void(const FJsonObject&, bool)> OnResponseCallback)
{
	FPendingRequest Req;
	{
		Req.RequestId = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
		Req.Verb = InVerb;
		Req.URL = InURL;
		Req.Http = HttpModule->CreateRequest();
		Req.Option = Option;
		Req.OnCallback = MoveTemp(OnResponseCallback);
	}


	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Req.Http.ToSharedRef();
	Request->OnProcessRequestComplete().BindLambda([this, RequestId = Req.RequestId](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (FPendingRequest* Found = ActiveRequests.Find(RequestId))
			{
				OnResponseReceived(*Found, Response, bWasSuccessful);
			}
		});

	Request->SetURL(InURL);
	Request->SetVerb(InVerb);

	//Set HTTP TimeOut
	if (Option.TimeoutSecond > -1)
		Request->SetTimeout(Option.TimeoutSecond);

	Request->SetHeader("Content-Type", TEXT("application/json"));
	//TODO : 현재는 Auth가 어떻게 들어올지 모르기 때문에 임의로 작업을 진행한다.
	FString Auth = GetAuth();
	if (Auth.IsEmpty())
		Request->SetHeader("Authorization", FString::Printf(TEXT("Basic %s"), *Auth));

	FString RequestBody;
	if (JsonObject)
	{
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	}

	Request->SetContentAsString(RequestBody);
	Request->ProcessRequest();

	Req.Tried++;
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

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		ActiveRequests.Remove(Request.RequestId);
		Request.OnCallback(*JsonObject, true);
	}		
}


//TODO : 추후 어떻게 Auth가 들어올지 모르기 때문에 우선적으로 러프하게 작업을 진행한다.
FString UHTTPHandler::GetAuth()
{

	return FString();
}
