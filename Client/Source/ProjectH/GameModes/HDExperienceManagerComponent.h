// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "HDExperienceManagerComponent.generated.h"

class UHDExperienceDefinition;

enum class EHDExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	ExecutingActions,
	Loaded,
	Deactivating
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnExperienceLoaded, const UHDExperienceDefinition*);

UCLASS()
class PROJECTH_API UHDExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
public:
	bool IsExperienceLoaded() const { return(LoadState == EHDExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

	//OnExperienceLoaded에 바인딩 하거나, 로딩이 완료되었다면 바로 호출함
	void CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate&& Delegate);

	void SetCurrentExperience(FPrimaryAssetId ExperienceId,FString SceneTableNo);
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();
	const UHDExperienceDefinition* GetCurrentExperienceChecked() const;

public:
	UPROPERTY()
	TObjectPtr<const UHDExperienceDefinition> CurrentExperience;
	FString CurrentSceneTableNo;

	//로딩 상태
	EHDExperienceLoadState LoadState = EHDExperienceLoadState::Unloaded;

	//로딩이 완료된 이후 Broadcasting Delegate
	FOnExperienceLoaded OnExperienceLoaded;

	//활성화된 GameFeature Plugin들
	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;
};
