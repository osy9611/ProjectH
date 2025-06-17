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
 * ī�޶�, �Է� �� �÷��̾ �����ϴ� �ý����� �ʱ�ȭ�� ó���ϴ� ������Ʈ
 * Pawn�� ���Ӽ��� �����ϱ� ���ؼ� ���������� ����
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class PROJECTH_API UHDHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UHDHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UHDHeroComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHDHeroComponent>() : nullptr); }

	static const FName NAME_ActorFeatureName;

	virtual void OnRegister() final;	//������Ʈ�� �����Ǵ� ���� �ʱ⿡ ȣ��Ǵ� �Լ�
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; } //Ư�� Feature�� ���� ���ͷ� ó���ϵ��� �ĺ��ϴ� �߿��� �κ�
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
