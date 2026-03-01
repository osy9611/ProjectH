// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularGame/Public/Sound/AudioSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "ModularGame/Public/Log/ModularLogChannel.h"
#include "ModularGame/Public/Data/ClientLocalStorageSubsystem.h"
#include "ModularGame/Public/Asset/ModularAssetManager.h"

void UAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ApplySoundMixOverrides();
}

void UAudioSubsystem::Deinitialize()
{
	BGMComponent = nullptr;
	//PendingBGMHandle.Reset();

	Super::Deinitialize();
}

void UAudioSubsystem::ApplySettings(const FSoundOptionData& InSoundOptionData)
{
	OptionData = InSoundOptionData;
	ApplySoundMixOverrides();
}

void UAudioSubsystem::PlayBGM(const TSoftObjectPtr<USoundBase>& Sound, float FadeInSeconds, float StartTime)
{
	CreateBGMComponent();
	if (!BGMComponent)
		return;

	//if (PendingBGMHandle.IsValid())
	//{
	//	PendingBGMHandle->CancelHandle();
	//	PendingBGMHandle.Reset();
	//}

	LoadSoundAsync(Sound, [this, Sound, FadeInSeconds, StartTime](USoundBase* Result)
		{
			if (!BGMComponent)
			{
				UE_LOG(Modular, Log, TEXT("[AudioSubsystem] PlayBGM LoadSoundAsync Fail BGMComponent is nullptr"));
				return;
			}

			if (!Result)
				return;

			if (BGMComponent->IsPlaying())
				BGMComponent->Stop();

			BGMComponent->SetSound(Result);
			BGMComponent->Play(StartTime);

			if (FadeInSeconds > 0.0f)
				BGMComponent->FadeIn(FadeInSeconds, 1.0f, StartTime);
		});
}

void UAudioSubsystem::StopBGM(float FadeOutSeconds)
{
	if (!BGMComponent)
		return;

	if (FadeOutSeconds > 0.0f && BGMComponent->IsPlaying())
		BGMComponent->FadeOut(FadeOutSeconds, 0.0f);
	else
		BGMComponent->Stop();
}

void UAudioSubsystem::PlaySFX2D(const TSoftObjectPtr<USoundBase>& Sound, float Volume, float Pitch)
{
	UWorld* World = GetWorldChecked();
	if (!World)
		return;

	LoadSoundAsync(Sound, [World, Volume, Pitch](USoundBase* Result)
		{
			if (!Result)
				return;

			UGameplayStatics::SpawnSound2D(World, Result, Volume, Pitch);
		});

}

void UAudioSubsystem::PlaySFXAtLocation(const TSoftObjectPtr<USoundBase>& Sound, const FVector& Location, float Volume, float Pitch)
{
	UWorld* World = GetWorldChecked();
	if (!World)
	{
		return;
	}
	LoadSoundAsync(Sound, [World, Location, Volume, Pitch](USoundBase* Result)
		{
			if (!Result)
				return;

			UGameplayStatics::SpawnSoundAtLocation(World, Result, Location, FRotator::ZeroRotator, Volume, Pitch);
		});
}

UWorld* UAudioSubsystem::GetWorldChecked() const
{
	UGameInstance* GI = GetGameInstance();
	return GI ? GI->GetWorld() : nullptr;
}

void UAudioSubsystem::CreateBGMComponent()
{
	if (BGMComponent)
		return;

	UWorld* World = GetWorldChecked();
	if (!World)
		return;

	BGMComponent = UGameplayStatics::SpawnSound2D(
		World,
		nullptr,   // Sound는 나중에 설정
		1.0f,
		1.0f,
		0.0f,
		nullptr,
		false,     // bPersistAcrossLevelTransition: 필요하면 true
		false      // bAutoDestroy
	);

	if (BGMComponent)
		BGMComponent->bAutoActivate = false;

}

void UAudioSubsystem::ApplySoundMixOverrides()
{
	UWorld* World = GetWorldChecked();
	if (!World || !GlobalMix || !MasterClass)
		return;

	UGameplayStatics::PushSoundMixModifier(World, GlobalMix);

	const float Master = OptionData.GetVolumeMute(ESoundType::Master) ? 0.0f : FMath::Clamp(OptionData.GetVolumeRatio(ESoundType::Master), 0.0f, 1.0f);
	const float BGM = OptionData.GetVolumeMute(ESoundType::BGM) ? 0.0f : FMath::Clamp(OptionData.GetVolumeRatio(ESoundType::BGM), 0.0f, 1.0f);
	const float SFX = OptionData.GetVolumeMute(ESoundType::SFX) ? 0.0f : FMath::Clamp(OptionData.GetVolumeRatio(ESoundType::SFX), 0.0f, 1.0f);

	UGameplayStatics::SetSoundMixClassOverride(World, GlobalMix, MasterClass, Master, 1.0f, 0.0f, true);

	if (BGMClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(World, GlobalMix, BGMClass, BGM, 1.0f, 0.0f, true);
	}
	if (SFXClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(World, GlobalMix, SFXClass, SFX, 1.0f, 0.0f, true);
	}

	UGameplayStatics::SetSoundMixClassOverride(World, GlobalMix, BGMClass, OptionData.GetVolumeRatio(ESoundType::BGM), 1.0f, 0.0f, true);
}

void UAudioSubsystem::LoadSoundAsync(const TSoftObjectPtr<USoundBase>& Sound, TFunction<void(USoundBase*)> OnLoaded)
{
	if (Sound.IsValid() && Sound.ToSoftObjectPath().IsNull())
	{
		OnLoaded(nullptr);
		return;
	}

	UModularAssetManager& AssetManager = UModularAssetManager::Get();
	AssetManager.GetAssetAsync<USoundBase>(Sound, OnLoaded,true);
}

