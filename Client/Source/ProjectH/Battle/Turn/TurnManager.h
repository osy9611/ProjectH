// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "ProjectH/Battle/HDBattleComponent.h"
#include "TurnManager.generated.h"

USTRUCT()
struct FTimelineEntry
{
	GENERATED_BODY()
public:
	UPROPERTY()
	UHDBattleComponent* BattleComp = nullptr;

	UPROPERTY()
	float NextTime = 0.0f;
};


UCLASS()
class PROJECTH_API UTurnManager : public UObject
{
	GENERATED_BODY()
public:
	void OnInitTurn(const TArray<AActor*> BattleActors);

	//다음 턴 이벤트를 실행
	void ExecuteNextTurn();

	void OnUpdateTurn();
	void AddBattleActor(const AActor* BattleActor, bool Immeidate);
	void RemoveBattleActor(const AActor* BattleActor);
	void RegisterStun(const AActor* BattleActor, float StunDuration);
	void UnRegisterStun(const AActor* BattleActor);

	void PullActorToFront(const TArray<AActor*> BattleActors);

	UHDBattleComponent* GetCurrentActor() const { return CurrentActor; }
	const TArray<FTimelineEntry>& GetTimeline() const { return Timeline; }

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnTurnStart, UHDBattleComponent*);
	FOnTurnStart OnTurnStart;

	float GetSpeed(UHDBattleComponent* Comp) { return FMath::Max(Comp->GetAttributeSet()->GetSpeed(), 1.0f); }
private:
	UPROPERTY()
	TArray<FTimelineEntry> Timeline;

	//스턴 전 원래 NextTime
	UPROPERTY()
	TMap<UHDBattleComponent*, float> PrevStunTime;

	UPROPERTY()
	UHDBattleComponent* CurrentActor;

	//전투 내 경과한 가상 시간
	float CurrentTime = 0.0f;
	//임계 게이지
	float Threshold = 10000.f;
};
