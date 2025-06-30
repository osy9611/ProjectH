// Fill out your copyright notice in the Description page of Project Settings.


#include "HDObjectPoolSubsystem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "ModularGame/Pool/Poolable_NiagaraSystem.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void UHDObjectPoolSubsystem::CreateParticle()
{
	CreatePool("Particle",
		[this](UWorld* World)
		{
			return NewObject<UParticleSystemComponent>(SpawnOwnerActor);
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


			ParticleComp->RegisterComponent();
			ParticleComp->bAutoActivate = false;
			ParticleComp->ActivateSystem(IsActive);
			ParticleComp->SetVisibility(IsActive);

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
PRAGMA_ENABLE_DEPRECATION_WARNINGS