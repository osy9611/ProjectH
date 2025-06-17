// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "HDPawnExtensionComponent.generated.h"

class UHDAbilitySystemComponent;
class UHDPawnData;
class UPaperZDAnimInstance;
class UPaperFlipbook;
class UPaperSprite;
UCLASS()
class PROJECTH_API UHDPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UHDPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	/*Feature Name Á¤ÀÇ*/
	static const FName NAME_ActorFeatureName;

	static UHDPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHDPawnExtensionComponent>() : nullptr); }

	template<class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UHDPawnData* InPawnData);
	void SetPlayerInputComponent();

	UHDAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }

	template<typename T = APlayerState>
	T* GetPlayerState()
	{
		APawn* Pawn = GetPawnChecked<APawn>();
		T* PlayerState = Pawn->GetPlayerState<T>();
		return PlayerState;
	}

	/*Actor Controller*/
	void SpawnDefaultController();

	/*GAS*/
	void InitializeGameAbilitySystem(AActor* IsOwnerActor);
	void RegisterGameAbilitySystem(UHDAbilitySystemComponent* InASC, AActor* IsOwnerActor);
	void UnRegisterGameAbilitySystem();

	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	/*GameFeature*/
	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void CheckDefaultInitialization() override;

	//Set Sprite
	void SetFlipbook(const FString Path);
	void SetFlipbook(UPaperFlipbook* NewFlipbook);
	//Set Anim
	void SetAnimationInstance(const FString Path);
	void SetAnimationInstance(TSubclassOf<UPaperZDAnimInstance> AnimInstance);

	//Set
	void SetFlipX(double dirX);

	UPROPERTY(EditDefaultsOnly, Category = "HD|Pawn")
	TObjectPtr<const UHDPawnData> PawnData;
	UPROPERTY()
	TObjectPtr<UHDAbilitySystemComponent> AbilitySystemComponent;

	UFUNCTION(BlueprintCallable)
	bool GetInteractionLock() { return IsInteractionLock; }

	UFUNCTION(BlueprintCallable)
	void SetInteractionLock(bool InteractionLock) { IsInteractionLock = InteractionLock; }

	UPROPERTY(EditDefaultsOnly, Category = "HD|Pawn")
	bool IsInteractionLock = false;

private:
	bool FacingRight = false;
};
