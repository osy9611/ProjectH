// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameplayCue_SkillHitEffect.h"
#include "NiagaraComponent.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectH/Util/UtilFunc_Pooling.h"
#include "ProjectH/Util//UtilFunc_Sprite.h"

void UHDGameplayCue_SkillHitEffect::PlayNiagara(UNiagaraSystem* NiagaraSystem, AActor* Target) const
{
}

void UHDGameplayCue_SkillHitEffect::PlayParticle(UParticleSystem* ParticleSystem, AActor* Target) const
{
	if (!Target)
		return;

	UParticleSystemComponent* ParticleComp = UtilFunc_Pooling::Get<UParticleSystemComponent>(GetWorld(), "Particle", false);
	if (!ParticleComp)
		return;

	ParticleComp->SetTemplate(ParticleSystem);
	ParticleComp->AttachToComponent(Target->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	ParticleComp->SetRelativeLocation(FVector::ZeroVector);

	ParticleComp->ActivateSystem(true);
	ParticleComp->SetVisibility(true);

	FVector SpriteSize = UtilFunc_Sprite::GetSpriteSize(Target);
	FVector CurrentLocation = Target->GetActorLocation();
	FVector NewLocation = CurrentLocation + SpriteSize;
	ParticleComp->AddLocalOffset(FVector(0.0f, 0.0f, NewLocation.Z));
}
