// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ModularAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class MODULARGAME_API UModularAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	UModularAssetManager();

	/*
	* UAssetManager's interfaces
	*/
	virtual void StartInitialLoading();

	static UModularAssetManager& Get();

	static bool ShouldLogAssetLoads();

	static UObject* SynchronusLoadAsset(const FSoftObjectPath& AssetPath);
	static void AsynchronusLoadAsset(const FSoftObjectPath& AssetPath, TFunction<void(UObject*)>callback);

	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepMemory = true);

	template<typename AssetType>
	static void GetAssetAsync(const TSoftObjectPtr<AssetType>& AssetPointer, TFunction<void(UObject*)>callback, bool bKeepInMemory = true);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template<typename AssetType>
	static void GetSubclassAsync(const TSoftClassPtr<AssetType>& AssetPointer, TFunction<void(TSubclassOf<AssetType>)>callback, bool bKeepInMemory = true);

	//[THREAD-SAFE] 메모리에 로딩된 에셋 캐싱
	void AddLoadedAsset(const UObject* Asset);

	//GC의 대상
	//언리얼에서는 레버런스 카운팅 시스템을 엔진 차원에서 자동 관리하기 때문에 Unity Addressable 시스템과는 다르게
	//가비지 컬렉션과 레퍼런스 카운팅 시스템 덕분에, 직접 메모리를 관리할 필요가 없어 로드된 에셋 존재 여부를 확인하거나 추적하는 데 사용한다.
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	//Object단위 Locking
	FCriticalSection SyncObject;
};

template<typename AssetType>
AssetType* UModularAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronusLoadAsset(AssetPath));

			//ensureAlwaysMsgf : 실행 중지하지 않고 오류 보고
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to loaded asset [%s]"), *AssetPointer.ToString());

			if (LoadedAsset && bKeepMemory)
			{
				//여기서 AddLoadAsset은 메모리에 상주하기 위한 장치라고 생각하면 됨
				// - 한번 등록되면 직접 내리지 않는한 Unloaded가 되지 않음(캐싱된다는 의미)
				Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
			}
		}
	}

	return LoadedAsset;
}

template <typename AssetType>
void UModularAssetManager::GetAssetAsync(const TSoftObjectPtr<AssetType>& AssetPointer, TFunction<void(UObject*)> callback, bool bKeepInMemory)
{
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		AssetType* LoadedAsset = nullptr;

		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			AsynchronusLoadAsset(AssetPath, [bKeepInMemory, callback](UObject* result)
				{
					if (result && bKeepInMemory)
						Get().AddLoadedAsset(result);

					callback(result);
				});
		}
	}
}

template<typename AssetType>
TSubclassOf<AssetType> UModularAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubClass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadedSubClass = AssetPointer.Get();
		if (!LoadedSubClass)
		{
			LoadedSubClass = Cast<UClass>(SynchronusLoadAsset(AssetPath));
			//ensureAlwaysMsgf : 실행 중지하지 않고 오류 보고
			ensureAlwaysMsgf(LoadedSubClass, TEXT("Failed to loaded asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubClass && bKeepInMemory)
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubClass));
	}

	return LoadedSubClass;
}

template<typename AssetType>
void UModularAssetManager::GetSubclassAsync(const TSoftClassPtr<AssetType>& AssetPointer, TFunction<void(TSubclassOf<AssetType>)> callback, bool bKeepInMemory)
{
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		UObject* resultAsset = nullptr;
		TSubclassOf<AssetType> LoadedSubClass = nullptr;
		LoadedSubClass = AssetPointer.Get();

		if (!LoadedSubClass)
		{
			AsynchronusLoadAsset(AssetPath, [resultAsset, bKeepInMemory]()
				{
					if (resultAsset && bKeepInMemory)
						Get().AddLoadedAsset(resultAsset);

					callback(Cast<UClass>(resultAsset));
				});
		}
	}
}