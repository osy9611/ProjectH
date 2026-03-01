// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularGame/Public/Asset/ModularAssetManager.h"
#include "ModularGame/Public/Log/ModularLogChannel.h"

UModularAssetManager::UModularAssetManager()
{
}

void UModularAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}

UModularAssetManager& UModularAssetManager::Get()
{
	check(GEngine);

	if (UModularAssetManager* Singletone = Cast<UModularAssetManager>(GEngine->AssetManager))
		return *Singletone;

	UE_LOG(Modular, Fatal, TEXT("invalid AssetManagerClassname in DefaultEngine.ini(project setting); it must be HakAssetManager"));

	//위의 UE_LOG의 Fatal로 인해, Crash 나기 때문에 도달하지 않지만 컴파일을 위해 더미를 리턴
	return *NewObject<UModularAssetManager>();
}

bool UModularAssetManager::ShouldLogAssetLoads()
{
	static bool bCached = false;
	static bool bLog = false;

	if (!bCached)
	{
		bLog = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
		bCached = true;
	}
	return bLog;
}

FName UModularAssetManager::DefaultKeepScope()
{
	static const FName GlobalScope(TEXT("Global"));
	return GlobalScope;
}

void UModularAssetManager::CacheLoadedAsset(UObject* Asset, FName Scope)
{
	if (!Asset || Scope.IsNone())
		return;

	FScopeLock Lock(&Mutex);
	FScopedObjectSet& SetRef = ScopedLoadedAssets.FindOrAdd(Scope);
	SetRef.Add(Asset);
}

void UModularAssetManager::ReleaseScope(FName Scope)
{
	if (Scope.IsNone())
		return;

	FScopeLock Lock(&Mutex);
	ScopedLoadedAssets.Remove(Scope);
}

void UModularAssetManager::CancelScopeHandles(FName Scope)
{
	if (Scope.IsNone())
		return;

	TArray<TSharedPtr<FStreamableHandle>> HandleToCancel;
	{
		FScopeLock Lock(&Mutex);

		if (TSet<TSharedPtr<FStreamableHandle>>* ScopeHandle = ScopeHandles.Find(Scope))
		{
			for (const TSharedPtr<FStreamableHandle>& Handle : *ScopeHandle)
			{
				HandleToCancel.Add(Handle);
			}
		}
	}

	for (const TSharedPtr<FStreamableHandle>& Handle : HandleToCancel)
	{
		if (Handle.IsValid() && !Handle->HasLoadCompleted())
			Handle->CancelHandle();
	}

	{
		FScopeLock Lock(&Mutex);
		ScopeHandles.Remove(Scope);
	}
}

void UModularAssetManager::CancelAllHandles()
{
	TArray<TSharedPtr<FStreamableHandle>> CancelHandles;

	{
		FScopeLock Lock(&Mutex);

		for (auto& Pair : ScopeHandles)
		{
			for (const TSharedPtr<FStreamableHandle>& Handle : Pair.Value)
			{
				CancelHandles.Add(Handle);
			}
		}

		ScopeHandles.Empty();
	}

	for (const TSharedPtr<FStreamableHandle>& Handle : CancelHandles)
	{
		if (Handle.IsValid() && !Handle->HasLoadCompleted())
			Handle->CancelHandle();
	}
}

void UModularAssetManager::ClearAllScopes()
{
	FScopeLock Lock(&Mutex);
	ScopedLoadedAssets.Empty();
}

UObject* UModularAssetManager::SyncLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (!AssetPath.IsValid())
		return nullptr;

	TUniquePtr<FScopeLogTime> LogTimePtr;
	if (ShouldLogAssetLoads())
	{
		LogTimePtr = MakeUnique<FScopeLogTime>(
				*FString::Printf(TEXT("Sync Load [%s]"),
				*AssetPath.ToString()),
				nullptr,
				FScopeLogTime::ScopeLog_Seconds);
	}

	check(UAssetManager::IsValid());
	return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
}

TSharedPtr<FStreamableHandle> UModularAssetManager::OnAsyncLoadAsset(const FSoftObjectPath& AssetPath, TFunction<void(UObject*)> Callback, TAsyncLoadPriority Priority)
{
	return Get().AsyncLoadAsset(AssetPath, MoveTemp(Callback), NAME_None, Priority);
}

TSharedPtr<FStreamableHandle> UModularAssetManager::AsyncLoadAsset(const FSoftObjectPath& AssetPath, TFunction<void(UObject*)> Callback, FName KeepScope, TAsyncLoadPriority Priority)
{
	if (!AssetPath.IsValid())
	{
		Callback(nullptr);
		return nullptr;
	}

	check(UAssetManager::IsValid());

	//이미 메모리에 있으면 Callback + 필요 시 캐싱
	if (UObject* Resolved = AssetPath.ResolveObject())
	{
		if (Resolved && KeepScope.IsNone())
			CacheLoadedAsset(Resolved, KeepScope);
		Callback(Resolved);
		return nullptr;
	}

	{
		FScopeLock Lock(&Mutex);

		//동일 Path 로딩 중이면 CallBack만 추가
		if (FInFlightRequest* Existing = InFlight.Find(AssetPath))
		{
			Existing->Callbacks.Add(MoveTemp(Callback));

			//스코프 정책:
			// - 여러 요청이 섞일 때 어떤 Scope로 캐시할지 애매해진다.
			// - 범용 코어에서는 "처음 들어온 Scope"를 유지하고, 이후는 캐시 Scope를 바꾸지 않는다.
			// - 더 고급 정책(LRU, Multi-Scope Retain)은 별도의 캐시 레이어에서 처리하는 편이 낫다/
			if (Existing->Scope.IsNone() && !KeepScope.IsNone())
			{
				Existing->Scope = KeepScope;
			}

			return Existing->Handle;
		}
	}

	//Create New Request
	TUniquePtr<FScopeLogTime> LogTimePtr;
	if (ShouldLogAssetLoads())
	{
		LogTimePtr = MakeUnique<FScopeLogTime>(
			*FString::Printf(TEXT("Async request [%s]"), *AssetPath.ToString()),
			nullptr,
			FScopeLogTime::ScopeLog_Seconds);
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(
		AssetPath,
		FStreamableDelegate::CreateUObject(this, &UModularAssetManager::CompleteInFlight, AssetPath),
		Priority);

	if (!Handle.IsValid())
	{
		Callback(nullptr);
		return nullptr;
	}

	//InFlight에 신규 요청 등록
	{
		FScopeLock Lock(&Mutex);

		FInFlightRequest& NewReq = InFlight.Add(AssetPath);
		NewReq.Handle = Handle;
		NewReq.Scope = KeepScope;
		NewReq.Callbacks.Add(MoveTemp(Callback));
	}

	TrackHandle(Handle, KeepScope);
		

	return Handle;
}

void UModularAssetManager::CompleteInFlight(const FSoftObjectPath AssetPath)
{
	UObject* Loaded = AssetPath.ResolveObject();

	FInFlightRequest ReqCopy;
	{
		FScopeLock Lock(&Mutex);

		FInFlightRequest* Req = InFlight.Find(AssetPath);
		if (!Req)
			return;

		ReqCopy = MoveTemp(*Req);
		InFlight.Remove(AssetPath);
	}

	//캐시 처리(스코프가 지정된 경우)
	if (Loaded && !ReqCopy.Scope.IsNone())
		CacheLoadedAsset(Loaded, ReqCopy.Scope);

	//Handle UnTrack (완료된 Handle 정리)
	if (ReqCopy.Handle.IsValid())
		UntrackHandle(ReqCopy.Handle, ReqCopy.Scope);

	//CallBack 호출
	for (TFunction<void(UObject*)>& Callback : ReqCopy.Callbacks)
	{
		Callback(Loaded);
	}
}

void UModularAssetManager::TrackHandle(const TSharedPtr<FStreamableHandle>& Handle, FName Scope)
{
	if (!Handle.IsValid())
		return;

	FScopeLock Lock(&Mutex);

	//캐시를 하지 않는(NAME_None) 로드라도, "Transient" 같은 스코프로 추적하고 싶을 수 있다.
	//범용 코어에서는 NAME_None이면 DefaultKeepScope가 아니라 "TransiendHandles"로 분리한다.
	const FName EffectiveScope = Scope.IsNone() ? FName(TEXT("TransientHandles")) : Scope;

	TSet<TSharedPtr<FStreamableHandle>>& ScopeHandle = ScopeHandles.FindOrAdd(EffectiveScope);
	ScopeHandle.Add(Handle);
}

void UModularAssetManager::UntrackHandle(const TSharedPtr<FStreamableHandle>& Handle, FName Scope)
{
	if (!Handle.IsValid())
		return;

	FScopeLock Lock(&Mutex);

	const FName EffectiveScope = Scope.IsNone() ? FName(TEXT("TransientHandles")) : Scope;

	if (TSet<TSharedPtr<FStreamableHandle>>* ScopeHandle = ScopeHandles.Find(EffectiveScope))
	{
		ScopeHandle->Remove(Handle);
		if (ScopeHandle->Num() == 0)
		{
			ScopeHandles.Remove(EffectiveScope);
		}
	}
}
