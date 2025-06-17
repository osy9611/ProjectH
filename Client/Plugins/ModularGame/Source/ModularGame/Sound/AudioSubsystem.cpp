// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioSubsystem.h"
#include "Components/AudioComponent.h"
#include "ModularGame/Log/ModularLogChannel.h"
#include "ModularGame/Data/ClientLocalStorageSubsystem.h"
#include "ModularGame/Asset/ModularAssetManager.h"

void UAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UAudioSubsystem::Deinitialize()
{
	AudioComponents.Empty();
	Super::Deinitialize();
}

void UAudioSubsystem::RegisterData()
{
	bool IsUpdateSoundOption = false;
	int32 MaxCount = static_cast<int32>(ESoundType::Max);
	for (int32 i = 0; i < MaxCount; ++i)
	{
		ESoundType SoundType = static_cast<ESoundType>(i);
		UAudioComponent* AudioComponent = GetAudioComponent(SoundType);
		if (!AudioComponent)
		{
			AudioComponent = NewObject<UAudioComponent>(this);
			AudioComponents.Add(SoundType, AudioComponent);
			IsUpdateSoundOption = true;
		}

		if (SoundType == ESoundType::BGM)
		{
			AudioComponent->bAutoActivate = true;
		}
		AudioComponent->RegisterComponentWithWorld(GetWorld());
		//AudioComponent->RegisterComponent();
	}

	if (IsUpdateSoundOption)
	{
		UpdateSoundOption();
	}
}

void UAudioSubsystem::UnRegisterData()
{
	int32 MaxCount = static_cast<int32>(ESoundType::Max);

	for (int32 i = 0; i < MaxCount; ++i)
	{
		ESoundType SoundType = static_cast<ESoundType>(i);
		UAudioComponent* AudioComponent = GetAudioComponent(SoundType);
		if (!AudioComponent)
		{
			continue;
		}
		AudioComponent->UnregisterComponent();
	}
}

void UAudioSubsystem::RegisterSoundOptionData(const FSoundOptionData& OptionData)
{
	SoundOptionData = OptionData;
	UpdateSoundOption();
}

void UAudioSubsystem::UpdateSoundOption()
{
	for (auto& AudioComponent : AudioComponents)
	{
		ESoundType SoundType = AudioComponent.Key;

		bool MuteOption = SoundOptionData.VolumeMutes[static_cast<int32>(SoundType)];
		if (SoundOptionData.MainVolumeMute || MuteOption)
		{
			AudioComponent.Value->SetVolumeMultiplier(0.0f);
		}
		else
		{
			float VolumeRatio = SoundOptionData.VolumeRatios[static_cast<int32>(SoundType)] * SoundOptionData.MainVolumeRatio;
			AudioComponent.Value->SetVolumeMultiplier(VolumeRatio);
		}
	}
}

void UAudioSubsystem::RegisterPlaySoundAfterSceneLoading(const FString SoundPath)
{
	RegisterBGMSound = SoundPath;
}

void UAudioSubsystem::PlaySoundAfterSceneLoading()
{
	if (RegisterBGMSound.IsEmpty())
		return;

	PlaySound2D_ByPath(ESoundType::BGM, RegisterBGMSound);
}

UAudioComponent* UAudioSubsystem::GetAudioComponent(ESoundType Type)
{
	if (UAudioComponent** AudioComponentPtr = AudioComponents.Find(Type))
	{
		if (UAudioComponent* AudioComponent = *AudioComponentPtr)
		{
			return AudioComponent;
		}
	}
	else
	{
		const UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESoundType"), true);
		FString TypeStr = Enum->GetNameStringByValue((int32)Type);
		UE_LOG(Modular, Log, TEXT("This Sound Type Not Have Component : %s"), *TypeStr);
	}

	return nullptr;
}

void UAudioSubsystem::SetMainVolume(float Volume)
{
	SoundOptionData.MainVolumeRatio = Volume;
	UpdateSoundOption();
}

void UAudioSubsystem::SetVolume(ESoundType Type, float Volume)
{
	if (UAudioComponent* AudioComponent = GetAudioComponent(Type))
	{
		AudioComponent->SetVolumeMultiplier(Volume);
		SoundOptionData.VolumeRatios[static_cast<int32>(Type)] = Volume;
		UpdateSoundOption();
	}
}

void UAudioSubsystem::PlaySound3D_ByPath(ESoundType Type, const FSoftObjectPath& BGMPath, FVector Location)
{
	if (UAudioComponent* AudioComponent = GetAudioComponent(Type))
	{
		/*UPTAssetManager& AssetManager = UPTAssetManager::Get();
		AssetManager.AsynchronusLoadAsset(BGMPath, [&, AudioComponent](UObject* result)
			{
				if (USoundBase* Sound = Cast<USoundBase>(result))
				{
					AudioComponent->SetSound(Sound);
					AudioComponent->SetWorldLocation(Location);
					AudioComponent->Play();
				}
			});*/
	}
}

void UAudioSubsystem::PlaySound3D_BySound(ESoundType Type, USoundBase* Sound, FVector Location)
{
	if (UAudioComponent* AudioComponent = GetAudioComponent(Type))
	{
		AudioComponent->SetSound(Sound);
		AudioComponent->SetWorldLocation(Location);
		AudioComponent->Play();
	}
}

void UAudioSubsystem::PlaySound2D_ByPath(ESoundType Type, const FSoftObjectPath& Path)
{
	if (UAudioComponent* AudioComponent = GetAudioComponent(Type))
	{
		UModularAssetManager& AssetManager = UModularAssetManager::Get();
		AssetManager.AsynchronusLoadAsset(Path, [&, AudioComponent](UObject* result)
			{
				if (USoundBase* Sound = Cast<USoundBase>(result))
				{
					AudioComponent->SetSound(Sound);
					AudioComponent->Play();
				}
			});
	}
}

void UAudioSubsystem::PlaySound2D_BySound(ESoundType Type, USoundBase* Sound)
{
	if (UAudioComponent* AudioComponent = GetAudioComponent(Type))
	{
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
}

void UAudioSubsystem::FadeInSound(ESoundType Type, float FadeDuration, float TargetVolume)
{
	if (UAudioComponent* AudioComponent = GetAudioComponent(Type))
	{
		AudioComponent->FadeIn(FadeDuration, TargetVolume);
	}
}

void UAudioSubsystem::FadeOutSound(ESoundType Type, float FadeDuration, float TargetVolume)
{
	if (UAudioComponent* AudioComponent = GetAudioComponent(Type))
	{
		AudioComponent->FadeOut(FadeDuration, TargetVolume);
	}
}
