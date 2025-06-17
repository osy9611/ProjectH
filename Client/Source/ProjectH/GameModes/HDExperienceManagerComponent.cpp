// Fill out your copyright notice in the Description page of Project Settings.


#include "HDExperienceManagerComponent.h"
#include "ProjectH/LogChannels.h"
#include "HDExperienceDefinition.h"
#include "HDExperienceActionSet.h"
#include "ModularGame/Asset/ModularAssetManager.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystemSettings.h"

void UHDExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
		Delegate.Execute(CurrentExperience);
	else
		//복사 비용을 낮추기 위해서는 Move를 하는 것을 잊지말자
		OnExperienceLoaded.Add(MoveTemp(Delegate));
}

void UHDExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId,FString SceneTableNo)
{
	UModularAssetManager& AssetManager = UModularAssetManager::Get();

	TSubclassOf<UHDExperienceDefinition> AssetClass;
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
		AssetClass = Cast<UClass>(AssetPath.TryLoad());

		UE_LOG(HDLog, Log, TEXT("[UPTExperienceManagerComponent] ExperienceID : %s"), *ExperienceId.ToString());
		UE_LOG(HDLog, Log, TEXT("[UPTExperienceManagerComponent] AssetPath : %s"), *AssetPath.ToString());
	}

	//CDO로 가져온 이유
	//객체를 직접 생성하지 않고도 클래스의 기본 데이터의 접근하기 위함
	const UHDExperienceDefinition* Experience = GetDefault<UHDExperienceDefinition>(AssetClass);
	check(Experience != nullptr);
	check(CurrentExperience == nullptr);
	{
		CurrentExperience = Experience;
		CurrentSceneTableNo = SceneTableNo;
	}

	StartExperienceLoad();
}

void UHDExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	check(LoadState == EHDExperienceLoadState::Unloaded);

	LoadState = EHDExperienceLoadState::Loading;

	UModularAssetManager& AssetManager = UModularAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	for (const TObjectPtr<UHDExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	TArray<FName> BundlesToLoad;
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}

	FStreamableDelegate OnAssetLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);

	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);

	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		FStreamableHandle::ExecuteDelegate(OnAssetLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetLoadedDelegate);
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetLoadedDelegate]()
			{
				OnAssetLoadedDelegate.ExecuteIfBound();
			}));

	}
}

void UHDExperienceManagerComponent::OnExperienceLoadComplete()
{
	check(LoadState == EHDExperienceLoadState::Loading);
	check(CurrentExperience);

	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
		{
			//FeaturePluginList를 순회하며, PluginURL을 ExperienceManagerComponent의 GameFeaturePluginURLs에 추가해준다.
			for (const FString& PluginName : FeaturePluginList)
			{
				FString PluginURL;
				if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
				{
					This->GameFeaturePluginURLs.AddUnique(PluginURL);
				}
			}
		};

	//GameFeatureToEnable에 있는 Plugin만 일단 활성화할 GameFeature Plugin 후보군으로 등록
	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);

	//GameFeaturePluginURLs에 등록된 Plugin을 로딩 및 활성화
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading)
	{
		LoadState = EHDExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			//매 Plugin이 로딩 및 활성화 이후, OnGameFeaturePluginLoadCompletes 콜백 함수 등록
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		OnExperienceFullLoadCompleted();
	}
}

void UHDExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	//매 GameFeature Plugin이 로딩될 때, 해당 함수가 콜백으로 불린다.
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading == 0)
	{
		//GameFeaturePlugin 로딩이 다 끝났을 경우, 기존대로 Loaded로서, OnExperienceFullLoadCompleted를 호출한다.
		//GameFeaturePluing 로딩과 활성화가 끝났다면? UGameFeatureAction을 활성화 해야한다.
		OnExperienceFullLoadCompleted();
	}
}

void UHDExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != EHDExperienceLoadState::Loaded);

	//GaemFeature Plugin의 로딩과 활성화 이후, GameFeature Action 들을 활성화 시켜야함
	{
		LoadState = EHDExperienceLoadState::ExecutingActions;
		FGameFeatureActivatingContext Context;
		{
			//월드의 핸들을 세팅해준다
			const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
			if (ExistingWorldContext)
				Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for (UGameFeatureAction* Action : ActionList)
				{
					//명시적으로 GameFeatureAction에 대해 Registering -> Loading -> Activation 순으로 호출한다
					if (Action)
					{
						Action->OnGameFeatureRegistering();
						Action->OnGameFeatureLoading();
						Action->OnGameFeatureActivating(Context);
					}
				}
			};

		ActivateListOfActions(CurrentExperience->Actions);

		for (const TObjectPtr<UHDExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	LoadState = EHDExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

const UHDExperienceDefinition* UHDExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EHDExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}
