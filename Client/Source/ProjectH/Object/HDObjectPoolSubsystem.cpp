// Fill out your copyright notice in the Description page of Project Settings.


#include "HDObjectPoolSubsystem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "ModularGame/Pool/Poolable_NiagaraSystem.h"

void UHDObjectPoolSubsystem::CreateParticle()
{
	CreatePool("Particle",
		[this](UWorld* World)
		{
			return NewObject<UParticleSystemComponent>(this);
		},
		5,
		[this](UObject* Obj, bool IsActive)
		{
			if (!Obj)
				return;

			UParticleSystemComponent* ParticleComp = Cast<UParticleSystemComponent>(Obj);
			if (!ParticleComp)
				return;

			ParticleComp->OnSystemFinished.AddDynamic(this, &UHDObjectPoolSubsystem::ReturnParticle);

			ParticleComp->SetVisibility(IsActive);
			ParticleComp->RegisterComponent();
			ParticleComp->Activate(IsActive);
		},
		[](UObject* Obj)
		{
			if (!Obj)
				return;

			UParticleSystemComponent* ParticleComp = Cast<UParticleSystemComponent>(Obj);
			if (!ParticleComp)
				return;

			ParticleComp->OnSystemFinished.Clear();

			ParticleComp->SetVisibility(false);
			ParticleComp->Activate(false);
			ParticleComp->UnregisterComponent();
		});
}

void UHDObjectPoolSubsystem::ReturnParticle(UParticleSystemComponent* Particle)
{
	if (!Particle)
		return;

	Return("Particle", Particle);
}
