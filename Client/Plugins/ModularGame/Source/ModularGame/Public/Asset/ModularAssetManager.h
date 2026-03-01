// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ModularAssetManager.generated.h"

struct FInFlightRequest
{
	TSharedPtr<FStreamableHandle> Handle;
	TArray<TFunction<void(UObject*)>> Callbacks;
	FName Scope; // 완료 시 캐시할 scope(원칙적으로 1개만 사용)
};

USTRUCT()
struct FScopedObjectSet
{
	GENERATED_BODY()

public:
	void Add(UObject* Asset)
	{
		if (!Asset)
			return;
		Objects.Add(Asset);
	}
private:
	UPROPERTY()
	TSet<TObjectPtr<UObject>> Objects;
};

UCLASS()
class MODULARGAME_API UModularAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	UModularAssetManager();

	/*
	* UAssetManager's interfaces
	*/
	virtual void StartInitialLoading() override;

	static UModularAssetManager& Get();
	static bool ShouldLogAssetLoads();

	// Sync Load Asset
	static UObject* SyncLoadAsset(const FSoftObjectPath& AssetPath);

	/*
	* Async Load
	*  - InFlight에서 동일 AssetPath 중복 요청
	*  - Callback은 항상 호출(성공/실패/이미 로드/Invalid 포함)
	*  - Handle 반환(추적/취소 가능)
	*/
	static TSharedPtr<FStreamableHandle> OnAsyncLoadAsset(const FSoftObjectPath& AssetPath, TFunction<void(UObject*)>callback, TAsyncLoadPriority Priority = FStreamableManager::DefaultAsyncLoadPriority);

	template<typename AssetType = UObject>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepMemory = true);

	template <typename AssetType = UObject>
	void GetAssetAsync(const TSoftObjectPtr<AssetType>& AssetPtr, TFunction<void(AssetType*)> Callback, bool bKeepInMemory = true, TAsyncLoadPriority Priority = FStreamableManager::DefaultAsyncLoadPriority);

	template<typename AssetType = UObject>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template<typename AssetType = UObject>
	static void GetSubclassAsync(const TSoftClassPtr<AssetType>& AssetPointer, TFunction<void(TSubclassOf<AssetType>)>callback, bool bKeepInMemory = true, TAsyncLoadPriority Priority = FStreamableManager::DefaultAsyncLoadPriority);

private:

	TSharedPtr<FStreamableHandle> AsyncLoadAsset(const FSoftObjectPath& AssetPath, TFunction<void(UObject*)> Callback, FName KeepScope, TAsyncLoadPriority Priority);
	void CompleteInFlight(const FSoftObjectPath AssetPath);
	void TrackHandle(const TSharedPtr<FStreamableHandle>& Handle, FName Scope);
	void UntrackHandle(const TSharedPtr<FStreamableHandle>& Handle, FName Scope);

	// Scope Caches
	static FName DefaultKeepScope();
	// [THREAD-SAFE] Asset을 특정 Scope에 캐시
	void CacheLoadedAsset(UObject* Asset, FName Scope);
	// [THREAD-SAFE] 특정 Scope 캐시 해제(GC 가능 상태로 전환)
	void ReleaseScope(FName Scope);
	// [THREAD-SAFE] 전체 Scope 캐시 해제
	void ClearAllScopes();
	// [THREAD-SAFE] 특정 Scope에서 발급된 Handle 전부 취소(로딩 중인 것만)
	void CancelScopeHandles(FName Scope);
	// [THREAD-SAFE] 모든 tracked handle 취소
	void CancelAllHandles();
private:
	// Scope 캐시: Scope -> set of objects (UPROPERTY로 GC 보호)
	UPROPERTY()
	TMap<FName, FScopedObjectSet> ScopedLoadedAssets;

	// InFlight: AssetPath -> request(Handle + callbacks)
	TMap<FSoftObjectPath, FInFlightRequest> InFlight;

	// Scope -> Handles (취소/정리/진단용)
	TMap<FName, TSet<TSharedPtr<FStreamableHandle>>> ScopeHandles;

	FCriticalSection Mutex;

};

template<typename AssetType>
AssetType* UModularAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepMemory)
{
	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetPtr.ToSoftObjectPath();

	if(!AssetPath.IsValid())
		return;

	LoadedAsset = AssetPtr.Get();
	if (!LoadedAsset)
	{
		LoadedAsset = Cast<AssetType>(SyncLoadAsset(AssetPath));
		ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset (sync) [%s]"), *AssetPtr.ToString());
	}

	if(LoadedAsset && bKeepMemory)
		Get().CacheLoadedAsset(LoadedAsset,DefaultKeepScope());

	return LoadedAsset;
}

template<typename AssetType>
void UModularAssetManager::GetAssetAsync(const TSoftObjectPtr<AssetType>& AssetPtr, TFunction<void(AssetType*)> Callback, bool bKeepInMemory, TAsyncLoadPriority Priority)
{
	const FSoftObjectPath& AssetPath = AssetPtr.ToSoftObjectPath();

	if (!AssetPath.IsValid())
	{
		Callback(nullptr);
		return;
	}

	if (AssetType* Asset = AssetPtr.Get())
	{
		if (bKeepInMemory)
			Get().CacheLoadedAsset(Asset, DefaultKeepScope());
		Callback(Asset);
		return;
	}

	FName Scope = bKeepInMemory ? DefaultKeepScope() : NAME_None;
	AsyncLoadAsset(AssetPath, [Callback, Scope, AssetPath](UObject* Result)
		{
			AssetType* Typed = Cast<AssetType>(Result);
			ensureAlwaysMsgf(!Result || Typed, TEXT("Type mismatch in GetAssetAsync: %s"), *AssetPath.ToString());

			if(Typed && !Scope.IsNone())
				Get().CacheLoadedAsset(Typed, Scope);

			Callback(Typed);
		},Scope, Priority);
}


template<typename AssetType>
TSubclassOf<AssetType> UModularAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPtr, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubClass = nullptr;
	const FSoftObjectPath& AssetPath = AssetPtr.ToSoftObjectPath();

	if (!AssetPath.IsValid())
		return nullptr;

	LoadedSubClass = AssetPtr.Get();
	if (!LoadedSubClass)
	{
		LoadedSubClass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
		ensureAlwaysMsgf(LoadedSubClass, TEXT("Failed to load class (sync) [%s]"), *AssetPtr.ToString());
	}

	if (LoadedSubClass && bKeepInMemory)
		Get().CacheLoadedAsset(LoadedSubClass.Get(), DefaultKeepScope());

	return LoadedSubClass;
}

template<typename AssetType>
void UModularAssetManager::GetSubclassAsync(const TSoftClassPtr<AssetType>& AssetPtr, TFunction<void(TSubclassOf<AssetType>)> Callback, bool bKeepInMemory, TAsyncLoadPriority Priority)
{
	const FSoftObjectPath& AssetPath = AssetPtr.ToSoftObjectPath();
	if (!AssetPath.IsValid())
	{
		Callback(nullptr);
		return;
	}

	//이미 로드된 경우 Callback 보장
	if (UClass* AssetClass = AssetPtr.Get())
	{
		if (bKeepInMemory)
			Get().CacheLoadedAsset(AssetClass, DefaultKeepScope());

		Callback(AssetClass);
		return;
	}

	FName Scope = bKeepInMemory ? DefaultKeepScope() : NAME_None;
	AsyncLoadAsset(AssetPath, [Callback, Scope,AssetPath](UObject* Result)
		{
			UClass* Typed = Cast<UClass>(Result);
			ensureAlwaysMsgf(!Result || Typed, TEXT("Type mismatch in GetSubclassAsync: %s"), *AssetPath.ToString());

			if(Typed && !Scope.IsNone())
				Get().CacheLoadedAsset(Typed,Scope);

			Callback(Result);
		},Scope, Priority);
}