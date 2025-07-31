// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Burst.h"
#include "HDGameplayCue_SkillHitEffect.generated.h"

class UNiagaraSystem;
class UParticleSystem;

UCLASS()
class PROJECTH_API UHDGameplayCue_SkillHitEffect : public UGameplayCueNotify_Burst
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void PlayNiagara(UNiagaraSystem* NiagaraSystem,AActor* Target) const;
	UFUNCTION(BlueprintCallable)
	void PlayParticle(UParticleSystem* ParticleSystem,AActor* Target) const;

};
