// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "ProjectH/Interaction/InteractInterface.h"
#include "ProjectH/Data/GenerateTableData.h"
#include "HDNPCComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadCompelteData, FNPCData, NPCData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogTriggered, int32, DialogID, AActor*, Interactor);

UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class PROJECTH_API UHDNPCComponent : public UPawnComponent, public IGameFrameworkInitStateInterface, public IInteractInterface
{
	GENERATED_BODY()
public:
	UHDNPCComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UHDNPCComponent* FindNPCComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHDNPCComponent>() : nullptr); }

	static const FName NAME_ActorFeatureName;

	virtual void OnRegister() final;	//컴포넌트가 생성되는 완전 초기에 호출되는 함수
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; } //특정 Feature를 가진 엑터로 처리하도록 식별하는 중요한 부분
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) final;
	virtual void CheckDefaultInitialization() final;

	//Interaction
	virtual void RegisterInteractionHandler();
	virtual void ExecuteInteraction(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable)
	void FinishInteraction();

	virtual AActor* GetInteractionActor() { return GetPawn<APawn>(); }
	virtual FVector GetActorLocation() override { return GetPawn<APawn>()->GetActorLocation(); }

	UPROPERTY(BlueprintAssignable, Category = "NPC")
	FOnDialogTriggered OnDialog;

	UPROPERTY(BlueprintAssignable, Category = "NPC")
	FLoadCompelteData OnLoadComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TableID;
};
