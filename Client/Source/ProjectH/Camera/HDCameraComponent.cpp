// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Containers/Set.h"

UHDCameraComponent::UHDCameraComponent(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UHDCameraComponent::OnRegister()
{
	Super::OnRegister();
}

void UHDCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!EnableOcclusion)
		return;
	if (TraceInterval > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::PerformOcclusionTrace, TraceInterval, true);
	}
}

void UHDCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTick)
{
	Super::TickComponent(DeltaTime, TickType, ThisTick);

	for (auto& Pair : OccludedMap)
	{
		FOccludedInfo& Info = Pair.Value;
		float Target = Info.IsCurrentOcculded ? OccludedAlpha : 1.0f;
		for (UMaterialInstanceDynamic* MID : Info.DynamicMats)
		{
			float Cur = 0.0f;
			MID->GetScalarParameterValue(TEXT("Fade"), Cur);
			float NewVal = FMath::FInterpTo(Cur, Target, DeltaTime, FadeInterpSpeed);
			MID->SetScalarParameterValue(TEXT("Fade"), NewVal);
		}
	}

	CleanupOcclusion();

	if (TraceInterval <= 0.0f)
		PerformOcclusionTrace();
}

void UHDCameraComponent::PerformOcclusionTrace()
{
	CurrentlyHit.Empty();

	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	FVector CamLoc = GetComponentLocation();
	FVector ActorLoc = Pawn->GetActorLocation() + TraceOffset;

	/*if (DebugTrace)
	{
		DrawDebugLine(GetWorld(), CamLoc, ActorLoc, FColor::Green, false, TraceInterval, 0, 1.f);
	}*/

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;

	bool IsHit = GetWorld()->SweepMultiByChannel(
		Hits,
		CamLoc,
		ActorLoc,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeSphere(TraceRadius),
		Params
	);

	if (DebugTrace)
	{
		for (const FHitResult& H : Hits)
		{
			DrawDebugSphere(GetWorld(), H.ImpactPoint, TraceRadius, 8, FColor::Red, false, TraceInterval);
		}
	}

	for (const FHitResult& Hit : Hits)
	{
		if (!Hit.GetActor())
			continue;

		if (!IsRelevantHit(Hit))
			continue;

		UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Hit.GetComponent());
		if (Mesh)
		{
			HideActor(Hit.GetActor(), Mesh);
			CurrentlyHit.Add(Hit.GetActor());
		}
	}

	for (auto& Pair : OccludedMap)
	{
		if (!CurrentlyHit.Contains(Pair.Key))
		{
			Pair.Value.IsCurrentOcculded = false;
		}
	}
}

void UHDCameraComponent::HideActor(AActor* Actor, UStaticMeshComponent* Mesh)
{
	FOccludedInfo* Info = OccludedMap.Find(Actor);
	if (!Info)
	{
		FOccludedInfo NewInfo;
		NewInfo.Mesh = Mesh;
		NewInfo.OriginalMats = Mesh->GetMaterials();
		for (int i = 0; i < Mesh->GetNumMaterials(); ++i)
		{
			UMaterialInstanceDynamic* MID = Mesh->CreateDynamicMaterialInstance(i, FadeMaterial);
			MID->SetScalarParameterValue(TEXT("Fade"), 1.f);
			NewInfo.DynamicMats.Add(MID);
		}
		OccludedMap.Add(Actor, MoveTemp(NewInfo));
		Info = OccludedMap.Find(Actor);
	}
	if (Info && !Info->IsCurrentOcculded)
	{
		Info->IsCurrentOcculded = true;
	}
}

void UHDCameraComponent::ShowActor(AActor* Actor)
{
	FOccludedInfo* Info = OccludedMap.Find(Actor);
	if (!Info)
		return;
	Info->IsCurrentOcculded = false;
}

bool UHDCameraComponent::IsRelevantHit(const FHitResult& Hit) const
{
	FVector PawnLoc = GetOwner()->GetActorLocation();
	if (Hit.ImpactPoint.Z < PawnLoc.Z - 20.f)
		return false;
	return true;
}

void UHDCameraComponent::CleanupOcclusion()
{
	TArray<TWeakObjectPtr<AActor>> ToRemove;
	for (auto& Pair : OccludedMap)
	{
		FOccludedInfo& Info = Pair.Value;
		// If not occluded and fully faded in
		bool bAllOpaque = true;
		for (UMaterialInstanceDynamic* MID : Info.DynamicMats)
		{
			float Cur = 0.f;
			MID->GetScalarParameterValue(TEXT("Fade"), Cur);
			if (Cur < 0.99f) { bAllOpaque = false; break; }
		}
		if (bAllOpaque && !Info.IsCurrentOcculded)
		{
			// restore original materials
			for (int i = 0; i < Info.OriginalMats.Num(); ++i)
			{
				if (Info.Mesh.IsValid())
					Info.Mesh->SetMaterial(i, Info.OriginalMats[i]);
			}
			ToRemove.Add(Pair.Key);
		}
	}
	// remove entries
	for (auto& ActorKey : ToRemove)
	{
		OccludedMap.Remove(ActorKey);
	}
}
