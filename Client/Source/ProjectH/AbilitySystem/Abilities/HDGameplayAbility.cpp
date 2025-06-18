// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameplayAbility.h"
#include "PaperFlipbook.h"
#include "PaperZDCharacter.h"
#include "ProjectH/Character/HDCharacter.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"
#include "ProjectH/Util/UtilFunc_Sprite.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/AbilitySystem/AbilityTask/AbilityTask_PlayFlipbookAndWait.h"

void UHDGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AActor* Actor = GetAvatarActorFromActorInfo();
	
	UBattleSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
	if (BattleSubsystem)
	{
		BattleSubsystem->CheckBattleState();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHDGameplayAbility::SetCameraMode(TSubclassOf<UModularCameraMode> CameraMode, bool UseFovOffset)
{
}

void UHDGameplayAbility::ClearCameraMode(bool UseFovOffest)
{
}

void UHDGameplayAbility::PlayFlipBookAnimation(FDynamicOnFlipbookComplete OnComplete)
{
	if (!AnimSequence)
		return;

	UAbilityTask_PlayFlipbookAndWait* Task = UAbilityTask_PlayFlipbookAndWait::PlayFlipbookAndWaitForEnd(this, AnimSequence);
	Task->OnCompleted.Add(OnComplete);
	Task->ReadyForActivation();
}
