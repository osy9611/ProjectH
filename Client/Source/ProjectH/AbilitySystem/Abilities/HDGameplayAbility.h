// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/AbilitySystem/Abilities/ModularGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "AnimSequences/PaperZDAnimSequence.h"
#include "HDGameplayAbility.generated.h"

class UModularCameraMode;
class UPaperFlipbook;

DECLARE_DYNAMIC_DELEGATE(FDynamicOnFlipbookComplete);

UCLASS()
class PROJECTH_API UHDGameplayAbility : public UModularGameplayAbility
{
	GENERATED_BODY()

public:

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void SetCameraMode(TSubclassOf<UModularCameraMode> CameraMode, bool UseFovOffset = true) override;
	virtual void ClearCameraMode(bool UseFovOffest = true) override;

	UFUNCTION(BlueprintCallable)
	void PlayFlipBookAnimation(FDynamicOnFlipbookComplete OnComplete);

	UFUNCTION()
	void OnAnimNotify(FName NotifyName);

public:
	/*UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "PaperFlipbook")
	UPaperFlipbook* Flipbook;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimSequence")
	UPaperZDAnimSequence* AnimSequence;
};
