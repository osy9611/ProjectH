// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectH/LogChannels.h"
#include "TODActor.generated.h"

class UDirectionalLightComponent;
class UTODHandler;

UENUM(BlueprintType)
enum class ETODHandleType : uint8
{
	SunMoon,
};


UCLASS()
class PROJECTH_API ATODActor : public AActor
{
	GENERATED_BODY()

public:
	ATODActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	void OnRegisterHandlers();
	void OnInitHandlers();
	
	void UpdateTime(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	bool InitSkySphereMaterail(UStaticMeshComponent* SkySphere, UMaterialInterface* SourceMaterial);
	
	UFUNCTION(BlueprintCallable)
	bool IsDayTime();
	UFUNCTION(BlueprintCallable)
	bool IsNightTime();

	template<typename T = UTODHandler>
	T* GetHandler(ETODHandleType Type);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DayLengthSeconds = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeOfDay = 9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DawnTime = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DuskTime = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SunMoonRotation = 0.0f;

	TObjectPtr<UMaterialInstance> SkySphereMaterialInstance;

	UPROPERTY()
	TMap<ETODHandleType, UTODHandler*> Handlers;
};

template<typename T>
T* ATODActor::GetHandler(ETODHandleType Type)
{
	if (UTODHandler* Handler = Handlers.FindKey(Type))
	{
		return Cast<T>(Handler);
	}
	return nullptr;
}
