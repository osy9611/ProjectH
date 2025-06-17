// Fill out your copyright notice in the Description page of Project Settings.


#include "Poolable_NiagaraSystem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"

void UPoolable_NiagaraSystem::SpawnRootActor(UWorld& InWorld)
{
	// 액터 스폰 위치 및 회전 값 설정
	FVector SpawnLocation = FVector(0.0f, 0.0f, 0.0f);
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	SpawnOwnerActor = InWorld.SpawnActor<AActor>(AActor::StaticClass(), SpawnLocation, SpawnRotation);

#if WITH_EDITOR
	SpawnOwnerActor->SetActorLabel(TEXT("NiagaraRoot"));
#endif

}
void UPoolable_NiagaraSystem::Init(int32 Count)
{
	for (int32 i = 0; i < Count; ++i)
	{
		if (UNiagaraComponent* NiagaraComp = Create())
		{
			NiagaraComp->Activate(false);
			ObjectPool.Add(NiagaraComp);
		}
	}
}

UNiagaraComponent* UPoolable_NiagaraSystem::Get(UNiagaraSystem* NiagaraSystem, AActor* OwnerActor, bool IsActive)
{
	UNiagaraComponent* NiagaraComp = nullptr;
	if (IsEmpty())
		NiagaraComp = Create();
	else
		NiagaraComp = ObjectPool.Pop();

	NiagaraComp->SetAsset(NiagaraSystem);
	NiagaraComp->bAutoActivate = false;
	NiagaraComp->AttachToComponent(OwnerActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	NiagaraComp->OnSystemFinished.AddDynamic(this, &ThisClass::Return);

	NiagaraComp->RegisterComponent();
	NiagaraComp->Activate(IsActive);

	return NiagaraComp;
}

void UPoolable_NiagaraSystem::Return(UNiagaraComponent* NiagaraComp)
{
	if (!NiagaraComp)
		return;
	NiagaraComp->SetAsset(nullptr);
	NiagaraComp->Deactivate();
	NiagaraComp->SetVisibility(false);
	NiagaraComp->OnSystemFinished.Clear();
	ObjectPool.Add(NiagaraComp);
}

UNiagaraComponent* UPoolable_NiagaraSystem::Create()
{
	return NewObject<UNiagaraComponent>(SpawnOwnerActor);
}