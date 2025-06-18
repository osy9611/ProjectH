// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PaperFlipbookComponent.h"
#include "PaperZDCharacter.h"
#include "PaperZDAnimInstance.h"
#include "AbilityTask_PlayFlipbookAndWait.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFlipbookFinished);

UCLASS()
class PROJECTH_API UAbilityTask_PlayFlipbookAndWait : public UAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnFlipbookFinished OnCompleted;

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "PlayFlipbookAndWait", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_PlayFlipbookAndWait* PlayFlipbookAndWaitForEnd(UGameplayAbility* OwningAbility,UPaperZDAnimSequence* Sequence);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

private:

	UPROPERTY()
	UPaperZDAnimInstance* AnimInstance;

	UPROPERTY()
	UPaperZDAnimSequence* AnimSequence;

	UFUNCTION()
	void OnFlipbookFinishedInternal();

	UPaperFlipbook* Flipbook;
};
