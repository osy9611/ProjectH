// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_PlayFlipbookAndWait.h"

UAbilityTask_PlayFlipbookAndWait* UAbilityTask_PlayFlipbookAndWait::PlayFlipbookAndWaitForEnd(UGameplayAbility* OwningAbility, UPaperZDAnimSequence* Sequence)
{
    APaperZDCharacter* Character = Cast<APaperZDCharacter>(OwningAbility->GetAvatarActorFromActorInfo());
    if (!Character)
    {
        return nullptr;
    }

    UAbilityTask_PlayFlipbookAndWait* NewTask = NewAbilityTask<UAbilityTask_PlayFlipbookAndWait>(OwningAbility);
    NewTask->AnimSequence = Sequence;
    NewTask->AnimInstance = Character->GetAnimInstance();

    return NewTask;
}

void UAbilityTask_PlayFlipbookAndWait::Activate()
{
    if(!AnimSequence)
    {
        EndTask();
        return;
    }

    APaperZDCharacter* Character = Cast<APaperZDCharacter>(Ability->GetAvatarActorFromActorInfo());
    if (!Character)
    {
        EndTask();
        return;
    }

    AnimInstance->PlayAnimationOverride(AnimSequence, "DefaultSlot", 1.0f, 0.0f,
        FZDOnAnimationOverrideEndSignature::CreateLambda([this](bool bCompleted)
            {
                OnFlipbookFinishedInternal();
            }));

}

void UAbilityTask_PlayFlipbookAndWait::OnDestroy(bool AbilityEnded)
{
    Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_PlayFlipbookAndWait::OnFlipbookFinishedInternal()
{
    if (ShouldBroadcastAbilityTaskDelegates())
        OnCompleted.Broadcast();

    AnimInstance->RestorePreMovieSequenceState();
    EndTask();
}
