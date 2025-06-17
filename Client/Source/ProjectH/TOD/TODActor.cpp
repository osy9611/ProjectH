// Fill out your copyright notice in the Description page of Project Settings.


#include "TODActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/DirectionalLightComponent.h"
#include "ProjectH/TOD/TODHandler_SunMoon.h"

// Sets default values
ATODActor::ATODActor(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void ATODActor::BeginPlay()
{
	Super::BeginPlay();

	OnRegisterHandlers();
	OnInitHandlers();
}

void ATODActor::OnRegisterHandlers()
{
	Handlers.Add(ETODHandleType::SunMoon, NewObject<UTODHandler_SunMoon>(this));
}

void ATODActor::OnInitHandlers()
{
	for (auto Pair : Handlers)
	{
		Pair.Value->OnInit(this);
	}
}

void ATODActor::UpdateTime(float DeltaTime)
{
	TimeOfDay += (DeltaTime / DayLengthSeconds) * 24.0f;

	if (Handlers.IsEmpty())
		return;
	for (auto Pair : Handlers)
	{
		if (Pair.Value->IsUpdate)
			Pair.Value->UpdateTime(DeltaTime);
	}
}

bool ATODActor::InitSkySphereMaterail(UStaticMeshComponent* SkySphere, UMaterialInterface* SourceMaterial)
{
	if (!SkySphere)
		return false;

	SkySphereMaterialInstance = SkySphere->CreateDynamicMaterialInstance(0, SourceMaterial);
	return IsValid(SkySphereMaterialInstance);
}

bool ATODActor::IsDayTime()
{
	return TimeOfDay > DawnTime - 0.3f && TimeOfDay < DuskTime + 0.3;
}

bool ATODActor::IsNightTime()
{
	return !IsDayTime();
}
