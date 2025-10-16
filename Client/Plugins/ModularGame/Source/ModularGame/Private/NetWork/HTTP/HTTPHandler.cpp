// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularGame/Public/Network/HTTP/HTTPHandler.h"
#include "ModularGame/Public/Log/ModularLogChannel.h"
#include "Runtime/Online/HTTP/Public/Http.h"

UHTTPHandler::UHTTPHandler(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HttpModule = &FHttpModule::Get();
}

void UHTTPHandler::OnCall_Get(const FString& InURL, TFunction<void(const FJsonObject&, bool)> OnResponseCallback)
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

	OnCall("GET", InURL, nullptr, OnResponseCallback);
}

void UHTTPHandler::OnCall_Post(const FString& InURL, const TSharedPtr<FJsonObject>& JsonObject, TFunction<void(const FJsonObject&, bool)> OnResponseCallback)
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

	OnCall("POST", InURL, JsonObject, OnResponseCallback);
}

void UHTTPHandler::OnCall(const FString& InVerb, const FString& InURL, const TSharedPtr<FJsonObject>& JsonObject, TFunction<void(const FJsonObject&, bool)> OnResponseCallback)
{
	//멀티스레드를 위해서 ThreadSafe로 적용함
	//ThreadSafe를 사용하면 Spin Lock이 내부에서 돌기 때문에 안전하게 스레드 접근이 가능하지만 속도는 느림
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule->CreateRequest();

	Request->OnProcessRequestComplete().BindLambda([OnResponseCallback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!bWasSuccessful || !Response.IsValid())
			{
				UE_LOG(Modular, Error, TEXT("[HTTPHandler] HTTP request failed. URL: %s"), *Request->GetURL());
				OnResponseCallback(FJsonObject(), false);
				return;
			}

			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				OnResponseCallback(*JsonObject, true);
			}
		});

	Request->SetURL(InURL);
	Request->SetVerb(InVerb);
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
}

//TODO : 추후 어떻게 Auth가 들어올지 모르기 때문에 우선적으로 러프하게 작업을 진행한다.
FString UHTTPHandler::GetAuth()
{

	return FString();
}
