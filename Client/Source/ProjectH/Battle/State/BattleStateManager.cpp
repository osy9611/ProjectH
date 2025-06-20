// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleStateManager.h"
#include "ProjectH/Battle/State/BattleState.h"
#include "ProjectH/Battle/State/BattleState_Ready.h"
#include "ProjectH/Battle/State/BattleState_Play.h"
#include "ProjectH/Battle/State/BattleState_Win.h"
#include "ProjectH/Battle/State/BattleState_Lose.h"
#include "ProjectH/Battle/State/BattleState_End.h"
#include "ProjectH/Battle/State/BattleState_CharTurn.h"
#include "ProjectH/Battle/State/BattleState_MonsterTurn.h"

UBattleStateManager::UBattleStateManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UBattleStateManager::OnInit()
{
	RegisterState();
	for (auto State : States)
	{
		State.Value->Initailize();
	}
}

void UBattleStateManager::OnStart()
{
	CurrentState = EBattleState::Ready;
	States[CurrentState]->DoStart();
}

void UBattleStateManager::OnAttackExecute(const FBattleStateParams& Params)
{
	if (CurrentState == EBattleState::Monster_Turn || CurrentState == EBattleState::Char_Turn)
	{
		States[CurrentState]->DoExecute(Params);
	}
}

void UBattleStateManager::CheckBattleState()
{
	if (CurrentState == EBattleState::Monster_Turn || CurrentState == EBattleState::Char_Turn)
	{
		States[CurrentState]->HandleEndSequence([this]()
			{
				ChangeState(EBattleState::Play);
			});
	}
}

void UBattleStateManager::RegisterState()
{
	States.Add(EBattleState::Ready, NewObject<UBattleState_Ready>(this));
	States.Add(EBattleState::Play, NewObject<UBattleState_Play>(this));
	States.Add(EBattleState::Char_Turn, NewObject<UBattleState_CharTurn>(this));
	States.Add(EBattleState::Monster_Turn, NewObject<UBattleState_MonsterTurn>(this));
	States.Add(EBattleState::Win, NewObject<UBattleState_Win>(this));
	States.Add(EBattleState::Lose, NewObject<UBattleState_Lose>(this));
	States.Add(EBattleState::End, NewObject<UBattleState_End>(this));
}

void UBattleStateManager::UpdateState(float DeltaTime)
{
	if (States.Contains(CurrentState))
	{
		States[CurrentState]->Update(DeltaTime);
	}
}

void UBattleStateManager::ChangeState(EBattleState NextState)
{
	if (CurrentState != NextState)
	{
		PrevState = CurrentState;
		States[PrevState]->DoEnd();

		CurrentState = NextState;
		States[CurrentState]->DoStart();
	}
}