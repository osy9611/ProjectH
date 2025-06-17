// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/Camera/ModularCameraComponent.h"
#include "HDCameraComponent.generated.h"

class UCapsuleComponent;

USTRUCT()
struct FOccludedInfo
{
	GENERATED_BODY()
public:
	TWeakObjectPtr<UStaticMeshComponent> Mesh;
	TArray<UMaterialInterface*> OriginalMats;
	TArray<UMaterialInstanceDynamic*> DynamicMats;
	bool IsCurrentOcculded = false;
};

UCLASS(Blueprintable, ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class PROJECTH_API UHDCameraComponent : public UModularCameraComponent
{
	GENERATED_BODY()
public:
	UHDCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTick) override;

private:
	void PerformOcclusionTrace();
	void HideActor(AActor* Actor, UStaticMeshComponent* Mesh);
	void ShowActor(AActor* Actor);

	bool IsRelevantHit(const FHitResult& Hit) const;

	void CleanupOcclusion();

public:
	UPROPERTY(EditAnywhere, Category = "Occlusion")
	bool EnableOcclusion = false;

	UPROPERTY(EditAnywhere,Category="Occlusion")
	bool DebugTrace;

	UPROPERTY(EditAnywhere, Category = "Occlusion")
	float TraceRadius = 12.f;

	UPROPERTY(EditAnywhere, Category = "Occlusion", meta = (ClampMin = "0.05", ClampMax = "1.0"))
	float TraceInterval = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Occlusion", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float OccludedAlpha = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Occlusion")
	float FadeInterpSpeed = 6.f;

	UPROPERTY(EditAnywhere, Category = "Occlusion")
	UMaterialInterface* FadeMaterial;

	UPROPERTY(EditAnywhere, Category = "Occlusion")
	FVector TraceOffset;
private:
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FOccludedInfo> OccludedMap;
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> CurrentlyHit;

	FTimerHandle TraceTimerHandle;
};
