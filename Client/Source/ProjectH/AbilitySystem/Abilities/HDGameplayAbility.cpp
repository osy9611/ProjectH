// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameplayAbility.h"
#include "PaperFlipbook.h"
#include "PaperZDCharacter.h"
#include "ProjectH/Character/HDCharacter.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"
#include "ProjectH/Util/UtilFunc_Sprite.h"
#include "ProjectH/Util/UtilFunc.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "ProjectH/AbilitySystem/HDAbilitySystemComponent.h"
#include "ProjectH/AbilitySystem/AbilityTask/AbilityTask_PlayFlipbookAndWait.h"

void UHDGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UHDAbilitySystemComponent* ASC = Cast<UHDAbilitySystemComponent>(ActorInfo->AbilitySystemComponent);
	if (ASC)
	{
		Params = ASC->ConsumeParams(Handle);
	}
}

void UHDGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AActor* Actor = GetAvatarActorFromActorInfo();

	UBattleSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
	if (BattleSubsystem)
	{
		BattleSubsystem->CheckBattleState();
	}
	Params = nullptr;
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

void UHDGameplayAbility::SetTargetOffSet(USceneComponent* SceneComp, AActor* Target)
{
	if (!SceneComp || !Target)
		return;

	FVector SpriteSize = UtilFunc_Sprite::GetSpriteSize(Target);
	FVector CurrentLocation = Target->GetActorLocation();
	FVector NewLocation = CurrentLocation + SpriteSize;
	SceneComp->AddLocalOffset(FVector(0.0f, 0.0f, NewLocation.Z));
}

UNiagaraComponent* UHDGameplayAbility::GetNiagaraComponent()
{
	if (!NiagaraSystem)
		return nullptr;

	AActor* Actor = GetAvatarActorFromActorInfo();
	return UtilFunc::GetNiagaraSystem(GetWorld(), NiagaraSystem, Actor, false);
}

FGameplayTag UHDGameplayAbility::GetGameplayTag()
{
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();

	if (Spec)
	{
		const FGameplayTagContainer& Tags = Spec->DynamicAbilityTags;
		return Tags.GetByIndex(0);
	}

	return FGameplayTag();
}