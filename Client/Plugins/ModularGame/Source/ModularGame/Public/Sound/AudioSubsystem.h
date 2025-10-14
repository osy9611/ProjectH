// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioSubsystem.generated.h"

struct FOptionData;

UENUM(BlueprintType)
enum class ESoundType : uint8
{
	BGM,
	SFX,
	Voice,
	Max
};

USTRUCT(BlueprintType)
struct FSoundOptionData
{
	GENERATED_BODY()

public:
	FSoundOptionData()
	{
		VolumeMutes.Init(false, static_cast<int32>(ESoundType::Max));
		VolumeRatios.Init(1.0f, static_cast<int32>(ESoundType::Max));
	}

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "MainVolumeMute")
	bool MainVolumeMute;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "VolumeMutes")
	TArray<bool> VolumeMutes;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "MainVolumeRatio")
	float MainVolumeRatio;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "VolumeRatios")
	TArray<float> VolumeRatios;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlaySoundAfterSceneLoading, const FString);
UCLASS(Blueprintable)
class MODULARGAME_API UAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void RegisterData();
	void UnRegisterData();

	void RegisterSoundOptionData(const FSoundOptionData& OptionData);

	UFUNCTION(BlueprintCallable)
	virtual void UpdateSoundOption();

	void RegisterPlaySoundAfterSceneLoading(const FString SoundPath);
	void PlaySoundAfterSceneLoading();

	UAudioComponent* GetAudioComponent(ESoundType Type);

	UFUNCTION(BlueprintCallable)
	void SetMainVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	void SetVolume(ESoundType Type, float Volume);

	UFUNCTION(BlueprintCallable)
	void PlaySound3D_ByPath(ESoundType Type, const FSoftObjectPath& BGMPath, FVector Location);

	UFUNCTION(BlueprintCallable)
	void PlaySound3D_BySound(ESoundType Type, USoundBase* Sound, FVector Location);

	UFUNCTION(BlueprintCallable)
	void PlaySound2D_ByPath(ESoundType Type, const FSoftObjectPath& Path);

	UFUNCTION(BlueprintCallable)
	void PlaySound2D_BySound(ESoundType Type, USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	void FadeInSound(ESoundType Type, float FadeDuration, float TargetVolume = 1.0f);

	UFUNCTION(BlueprintCallable)
	void FadeOutSound(ESoundType Type, float FadeDuration, float TargetVolume = 0.0f);

	UPROPERTY(BlueprintReadOnly, Category = "SoudOptionData")
	FSoundOptionData SoundOptionData;
private:
	UPROPERTY()
	FString RegisterBGMSound;

	UPROPERTY()
	TMap<ESoundType, UAudioComponent*> AudioComponents;
};
