// Fill out your copyright notice in the Description page of Project Settings.


#include "HDInputManagerSubsystem.h"
#include "CommonLocalPlayer.h"
#include "CommonPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/Input/HDInputConfig.h"
#include "ProjectH/Util/UtilFunc.h"
UHDInputManagerSubsystem::UHDInputManagerSubsystem()
{
}

void UHDInputManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UHDInputManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UHDInputManagerSubsystem::NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer)
{
	check(LocalPlayer);

	LocalPlayer->OnPlayerContollerSet.AddWeakLambda(this, [this](UCommonLocalPlayer* LocalPlayer, APlayerController* PlayerController)
		{
			if (LocalPlayer)
			{
				ACommonPlayerController* CommonPlayerController = Cast<ACommonPlayerController>(PlayerController);

				NotifyPlayerRemoved(LocalPlayer);
			}
		});
}

void UHDInputManagerSubsystem::NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer)
{
	RemoveAllInputContexts();
}

void UHDInputManagerSubsystem::NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer)
{
	check(LocalPlayer);

	LocalPlayer->OnPlayerContollerSet.RemoveAll(this);
	RemoveAllInputContexts();
}

void UHDInputManagerSubsystem::RegisterHDMappableConfigPair(const FHDMappableConfigPair& Pair)
{
	UEnhancedInputLocalPlayerSubsystem* EnhanceSubsystem = GetEnhanceSubsystem();
	if (!EnhanceSubsystem)
		return;

	if (Pair.bShouldActivateAutomatically)
	{
		//Enhanced Input 시스템에서 입력 컨택스트를 추가하거나 제거할 때 사용되는 구조체
		FModifyContextOptions Options = {};
		Options.bIgnoreAllPressedKeysUntilRelease = false;

		//내부적으로 Input Mapping Context를 추가한다:
		EnhanceSubsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
	}
}

void UHDInputManagerSubsystem::RegisterHDMappableConfigPairs(const TArray<FHDMappableConfigPair>& Pairs)
{
	UEnhancedInputLocalPlayerSubsystem* EnhanceSubsystem = GetEnhanceSubsystem();
	if (!EnhanceSubsystem)
		return;

	for (const FHDMappableConfigPair& Pair : Pairs)
	{
		if (Pair.bShouldActivateAutomatically)
		{
			//Enhanced Input 시스템에서 입력 컨택스트를 추가하거나 제거할 때 사용되는 구조체
			FModifyContextOptions Options = {};
			Options.bIgnoreAllPressedKeysUntilRelease = false;

			//내부적으로 Input Mapping Context를 추가한다:
			EnhanceSubsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
		}
	}
}

void UHDInputManagerSubsystem::LoadInputContext(EInputMappingType Type, bool AutoApply)
{
	FString InputContextPath = HDInputContainer::GetMappingContextPath(Type);
	if (InputContextPath.IsEmpty())
	{
		UE_LOG(HDLog, Log, TEXT("[HDInputManagerSubsystem] Path Not Found Type "));
		return;
	}

	UInputMappingContext* InputContext = UtilFunc::LoadResource<UInputMappingContext>(InputContextPath);
	if (!InputContext)
	{
		UE_LOG(HDLog, Log, TEXT("[HDInputManagerSubsystem] InputMappingContext Load Fail %s"), *InputContextPath);
		return;
	}

	AddInputContext(InputContext);

	if (AutoApply)
	{
		ApplyContext(InputContext, 0);
	}
}

void UHDInputManagerSubsystem::AddInputContext(UInputMappingContext* Context)
{
	if (!Context)
	{
		UE_LOG(HDLog, Log, TEXT("[HDInputManagerSubsystem] AddInputContext Fail Context is nullptr"));
		return;
	}

	MappingContexts.Add(Context);
}

void UHDInputManagerSubsystem::RemoveInputContext(UInputMappingContext* Context)
{
	UEnhancedInputLocalPlayerSubsystem* EnhanceSubsystem = GetEnhanceSubsystem();
	if (!EnhanceSubsystem)
		return;

	EnhanceSubsystem->RemoveMappingContext(Context);
	ActiveMappings.Remove(Context);
}

void UHDInputManagerSubsystem::RemoveAllInputContexts()
{
	UEnhancedInputLocalPlayerSubsystem* EnhanceSubsystem = GetEnhanceSubsystem();
	if (!EnhanceSubsystem)
		return;

	EnhanceSubsystem->ClearAllMappings();
	ActiveMappings.Empty();
	MappingContexts.Empty();
}

void UHDInputManagerSubsystem::ApplyContext(UInputMappingContext* Context, int32 Priority)
{
	UEnhancedInputLocalPlayerSubsystem* EnhanceSubsystem = GetEnhanceSubsystem();
	if (!EnhanceSubsystem)
		return;

	if (!MappingContexts.Contains(Context))
	{
		UE_LOG(HDLog, Error, TEXT("[HDInputManagerSubsystem] ApplyContext Fail MappingData Not Found"));
		return;
	}

	//Enhanced Input 시스템에서 입력 컨택스트를 추가하거나 제거할 때 사용되는 구조체
	FModifyContextOptions Options = {};
	Options.bIgnoreAllPressedKeysUntilRelease = false;

	EnhanceSubsystem->AddMappingContext(Context, Priority, Options);
	ActiveMappings.Add(Context);
}

UEnhancedInputLocalPlayerSubsystem* UHDInputManagerSubsystem::GetEnhanceSubsystem()
{
	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	if (!LocalPlayer)
		return nullptr;

	UEnhancedInputLocalPlayerSubsystem* EnhanceSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!EnhanceSubsystem)
	{
		UE_LOG(HDLog, Error, TEXT("[HDInputManagerSubsystem]  EnhanceSubsystem is nullptr"));
		return nullptr;
	}

	return EnhanceSubsystem;
}
