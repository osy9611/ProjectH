// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnManager.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Player/HDPlayerState.h"
#include "Math/UnrealMathUtility.h" 

void UTurnManager::OnInitTurn(const TArray<AActor*> BattleActors)
{
	//Reset Timeline
	Timeline.Empty();
	CurrentTime = 0.0f;

	//Add TurnQueue
	for (const AActor* Actor : BattleActors)
	{
		if (!IsValid(Actor))
			continue;

		UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(Actor);

		if (!BattleComp)
			continue;

		float Speed = GetSpeed(BattleComp);
		float BaseTime = Threshold / Speed;

		FTimelineEntry NewTimeline;
		NewTimeline.BattleComp = BattleComp;
		NewTimeline.NextTime = BaseTime;
		Timeline.Add(NewTimeline);
	}

	OnUpdateTurn();
}

void UTurnManager::ExecuteNextTurn()
{
	if (Timeline.Num() == 0)
		return;

	//가장 빠른 엔드리 꺼내기
	FTimelineEntry Entry = Timeline[0];
	Timeline.RemoveAt(0);

	//가상 시간 동기화
	CurrentTime = Entry.NextTime;
	CurrentActor = Entry.BattleComp;

	OnTurnStart.Broadcast(Entry.BattleComp);

	float Speed = GetSpeed(Entry.BattleComp);
	float BaseTime = Threshold / Speed;
	Entry.NextTime = CurrentTime + BaseTime;

	Timeline.Add(Entry);
	OnUpdateTurn();
}

void UTurnManager::OnUpdateTurn()
{
	Timeline.Sort([](const FTimelineEntry& A, const FTimelineEntry& B)
		{
			if (A.NextTime < B.NextTime)
				return true;

			if (A.NextTime == B.NextTime)
			{
				if (A.BattleComp->CharType == B.BattleComp->CharType)
				{
					return A.BattleComp->SlotNo < B.BattleComp->SlotNo;
				}
				else
				{
					if (A.BattleComp->CharType == ECharType::Character)
						return true;
				}
			}
			return false;
		});
}

void UTurnManager::AddBattleActor(const AActor* BattleActor, bool Immeidate)
{
	if (!IsValid(BattleActor))
		return;

	UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(BattleActor);

	if (!BattleComp)
		return;

	float Speed = GetSpeed(BattleComp);
	float BaseTime = Threshold / Speed;
	float InsertTime = 0;
	if (Immeidate)
	{
		InsertTime = CurrentTime;
	}
	else
	{
		InsertTime = Timeline.Num() > 0 ? Timeline.Last().NextTime : CurrentTime;
	}
	FTimelineEntry NewTimeline;
	NewTimeline.BattleComp = BattleComp;
	NewTimeline.NextTime = BaseTime;
	Timeline.Add(NewTimeline);
	OnUpdateTurn();
}

void UTurnManager::RemoveBattleActor(const AActor* BattleActor)
{
	if (IsValid(BattleActor))
		return;

	UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(BattleActor);

	if (!BattleComp)
		return;

	Timeline.RemoveAll([&](const FTimelineEntry& Entry)
		{
			return Entry.BattleComp == BattleComp;
		});
}

void UTurnManager::RegisterStun(const AActor* BattleActor, float StunDuration)
{
	if (!IsValid(BattleActor))
		return;

	UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(BattleActor);

	if (!BattleComp)
		return;

	FTimelineEntry* Entry = Timeline.FindByPredicate([&](const FTimelineEntry& E)
		{
			return E.BattleComp == BattleComp;
		});

	if (Entry)
	{
		PrevStunTime.Add(BattleComp, Entry->NextTime);
		Entry->NextTime = FMath::Max(Entry->NextTime, CurrentTime) + StunDuration;
	}

	OnUpdateTurn();

}

void UTurnManager::UnRegisterStun(const AActor* BattleActor)
{
	if (!IsValid(BattleActor))
		return;

	UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(BattleActor);

	if (!BattleComp)
		return;

	FTimelineEntry* Entry = Timeline.FindByPredicate([&](const FTimelineEntry& E)
		{
			return E.BattleComp == BattleComp;
		});

	if (!Entry)
		return;
	
	if (PrevStunTime.Contains(BattleComp))
	{
		float OriginTime = PrevStunTime[BattleComp];
		
		//이전 시간으로 복원
		Entry->NextTime = OriginTime;

		PrevStunTime.Remove(BattleComp);
		OnUpdateTurn();
	}
}

void UTurnManager::PullActorToFront(const TArray<AActor*> BattleActors)
{
	if (BattleActors.IsEmpty())
		return;

	for (const AActor* BattleActor : BattleActors)
	{
		UHDBattleComponent* BattleComp = UHDBattleComponent::FindBattleComponent(BattleActor);

		if (!BattleComp)
			continue;

		FTimelineEntry* Entry = Timeline.FindByPredicate([&](const FTimelineEntry& E)
			{
				return E.BattleComp == BattleComp;
			});

		if (!Entry)
			continue;

		Entry->NextTime = CurrentTime;
	}

	OnUpdateTurn();
}
