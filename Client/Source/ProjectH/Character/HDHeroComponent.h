// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "ProjectH/Input/HDMappableConfigPair.h"
#include "HDHeroComponent.generated.h"

struct FHDMappableConfigPair;
struct FInputActionValue;
class UModularCameraMode;
class UInteractionComponent;
class UHDInputConfig;
/**
 * 카메라, 입력 등 플레이어가 제어하는 시스템의 초기화를 처리하는 컴포넌트
 * Pawn에 종속성을 방지하기 위해서 독립적으로 만듬
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class PROJECTH_API UHDHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UHDHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UHDHeroComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHDHeroComponent>() : nullptr); }

	static const FName NAME_ActorFeatureName;

	virtual void OnRegister() final;	//컴포넌트가 생성되는 완전 초기에 호출되는 함수
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; } //특정 Feature를 가진 엑터로 처리하도록 식별하는 중요한 부분
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) final;
	virtual void CheckDefaultInitialization() final;

	//AbilityCameraSet
	void SetAbilityCameraMode(TSubclassOf<UModularCameraMode> CameraMode, FGameplayAbilitySpecHandle& OwningSpecHandle, bool UseFovOffset = true);
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle, bool UseFovOffset = true);

	TSubclassOf<UModularCameraMode> DetermineCameraMode() const;

	//Input
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	void RegisterInputAction(UHDInputConfig* InputConfig);

	void SetMove(bool IsMove) { bIsMove = IsMove; }
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Interaction(const FInputActionValue& InputActionValue);
	void Input_TEST(const FInputActionValue& InputActionValue);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	//Interaction
	UFUNCTION(BlueprintCallable)
	void RegisterInteractionComponent(UInteractionComponent* Component);

public:
	UPROPERTY(EditAnywhere)
	bool bIsMove = true;

	UPROPERTY()
	UInteractionComponent* InteractionComponent;

	UPROPERTY(EditAnywhere)
	TArray<FHDMappableConfigPair> DefaultInputConfigs;

	UPROPERTY()
	TSubclassOf<UModularCameraMode> AbilityCameraMode;

	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;
};
