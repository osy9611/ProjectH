// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGame/AbilitySystem/Abilities/ModularGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "AnimSequences/PaperZDAnimSequence.h"
#include "HDGameplayAbility.generated.h"

class UModularCameraMode;
class UPaperFlipbook;
class UNiagaraSystem;
class UNiagaraComponent;
struct FGameAbilityParam;

DECLARE_DYNAMIC_DELEGATE(FDynamicOnFlipbookComplete);

UCLASS()
class PROJECTH_API UHDGameplayAbility : public UModularGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void SetCameraMode(TSubclassOf<UModularCameraMode> CameraMode, bool UseFovOffset = true) override;
	virtual void ClearCameraMode(bool UseFovOffest = true) override;

	UFUNCTION(BlueprintCallable)
	virtual void PlayFlipBookAnimation(FDynamicOnFlipbookComplete OnComplete);

	UFUNCTION(BlueprintCallable)
	void SetNiagaraSystem(UNiagaraSystem* InNiagaraSystem) { NiagaraSystem = InNiagaraSystem; }

	UFUNCTION(BlueprintCallable)
	virtual UNiagaraComponent* GetNiagaraComponent();

protected:
	virtual FGameplayTag GetGameplayTag();
public:
	/*UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "PaperFlipbook")
	UPaperFlipbook* Flipbook;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimSequence")
	UPaperZDAnimSequence* AnimSequence;

protected:
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
	FGameAbilityParam* Params;
};
