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
		//���� ����� ���߱� ���ؼ��� Move�� �ϴ� ���� ��������
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

	//CDO�� ������ ����
	//��ü�� ���� �������� �ʰ� Ŭ������ �⺻ �������� �����ϱ� ����
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
			//FeaturePluginList�� ��ȸ�ϸ�, PluginURL�� ExperienceManagerComponent�� GameFeaturePluginURLs�� �߰����ش�.
			for (const FString& PluginName : FeaturePluginList)
			{
				FString PluginURL;
				if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
				{
					This->GameFeaturePluginURLs.AddUnique(PluginURL);
				}
			}
		};

	//GameFeatureToEnable�� �ִ� Plugin�� �ϴ� Ȱ��ȭ�� GameFeature Plugin �ĺ������� ���
	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);

	//GameFeaturePluginURLs�� ��ϵ� Plugin�� �ε� �� Ȱ��ȭ
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading)
	{
		LoadState = EHDExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			//�� Plugin�� �ε� �� Ȱ��ȭ ����, OnGameFeaturePluginLoadCompletes �ݹ� �Լ� ���
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
	//�� GameFeature Plugin�� �ε��� ��, �ش� �Լ��� �ݹ����� �Ҹ���.
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading == 0)
	{
		//GameFeaturePlugin �ε��� �� ������ ���, ������� Loaded�μ�, OnExperienceFullLoadCompleted�� ȣ���Ѵ�.
		//GameFeaturePluing �ε��� Ȱ��ȭ�� �����ٸ�? UGameFeatureAction�� Ȱ��ȭ �ؾ��Ѵ�.
		OnExperienceFullLoadCompleted();
	}
}

void UHDExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != EHDExperienceLoadState::Loaded);

	//GaemFeature Plugin�� �ε��� Ȱ��ȭ ����, GameFeature Action ���� Ȱ��ȭ ���Ѿ���
	{
		LoadState = EHDExperienceLoadState::ExecutingActions;
		FGameFeatureActivatingContext Context;
		{
			//������ �ڵ��� �������ش�
			const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
			if (ExistingWorldContext)
				Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for (UGameFeatureAction* Action : ActionList)
				{
					//��������� GameFeatureAction�� ���� Registering -> Loading -> Activation ������ ȣ���Ѵ�
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
