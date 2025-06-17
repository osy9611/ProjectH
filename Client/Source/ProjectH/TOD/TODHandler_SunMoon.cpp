// Fill out your copyright notice in the Description page of Project Settings.


#include "TODHandler_SunMoon.h"
#include "Components/DirectionalLightComponent.h"
#include "ProjectH/TOD/TODActor.h"

UTODHandler_SunMoon::UTODHandler_SunMoon()
{
	IsUpdate = true;
}

void UTODHandler_SunMoon::OnInit(ATODActor* Actor)
{
	Super::OnInit(Actor);

	if (!Actor)
		return;
	TArray<UDirectionalLightComponent*> ComDirections;
	Actor->GetComponents<UDirectionalLightComponent>(ComDirections);

	if (ComDirections.IsEmpty())
		return;

	for (UDirectionalLightComponent* ComDirection : ComDirections)
	{
		if (!ComDirection)
			continue;

		if (ComDirection->GetFName() == "SunDirectionalLight")
		{
			SunDirectionLight = ComDirection;
		}

		if (ComDirection->GetFName() == "MoonDirctionLIght")
		{
			MoonDirectionLight = ComDirection;
		}
	}
}

void UTODHandler_SunMoon::HandleSunMoonRotation()
{
	if (SunDirectionLight)
	{
		float SunClampResult = FMath::GetMappedRangeValueClamped(FVector2D(Owner->DawnTime, Owner->DuskTime), FVector2D(0.0f, -180.0f), Owner->TimeOfDay);
		SunDirectionLight->SetWorldRotation(FRotator(0.0f, SunClampResult, Owner->SunMoonRotation));
	}

	if (MoonDirectionLight)
	{
		float RotationAfterDusk = FMath::GetMappedRangeValueClamped(FVector2D(Owner->DuskTime, 24.0f), FVector2D(-180.0f, -90.0f), Owner->TimeOfDay);
		MoonDirectionLight->SetWorldRotation(FRotator(0.0f, RotationAfterDusk, Owner->SunMoonRotation));

		float RotationBeforeDusk = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, Owner->DawnTime), FVector2D(-90.0f, 0.0f), Owner->TimeOfDay);
		MoonDirectionLight->SetWorldRotation(FRotator(0.0f, RotationAfterDusk, Owner->SunMoonRotation));
	}
}

void UTODHandler_SunMoon::HandleVisibility()
{
	if (SunDirectionLight)
	{
		SunDirectionLight->SetVisibility(Owner->IsDayTime(), false);
	}

	if (MoonDirectionLight)
	{
		MoonDirectionLight->SetVisibility(Owner->IsNightTime(), false);
	}
}
