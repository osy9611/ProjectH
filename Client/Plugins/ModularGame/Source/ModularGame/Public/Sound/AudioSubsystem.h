// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioSubsystem.generated.h"

struct FOptionData;

UENUM(BlueprintType)
enum class ESoundType : uint8
{
	Master,
	BGM,
	SFX,
	Voice,
	UI,
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

	bool GetVolumeMute(ESoundType Type) { return VolumeMutes[(int32)Type]; }

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "VolumeMutes")
	TArray<bool> VolumeMutes;

	float GetVolumeRatio(ESoundType Type) { return  VolumeRatios[(int32)Type]; }

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "VolumeRatios")
	TArray<float> VolumeRatios;
};

UCLASS(Blueprintable)
class MODULARGAME_API UAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void ApplySettings(const FSoundOptionData& InSoundOptionData);
	const FSoundOptionData& GetOptionData() const { return OptionData; }

	void PlayBGM(const TSoftObjectPtr<USoundBase>& Sound, float FadeInSeconds = 0.5f, float StartTime = 0.0f);
	void StopBGM(float FadeOutSeconds);
	void PlaySFX2D(const TSoftObjectPtr<USoundBase>& Sound, float Volume = 1.0f, float Pitch = 1.0f);
	void PlaySFXAtLocation(const TSoftObjectPtr<USoundBase>& Sound, const FVector& Location, float Volume = 1.0f, float Pitch = 1.0f);

private:
	UWorld* GetWorldChecked() const;

	void CreateBGMComponent();
	void ApplySoundMixOverrides();

	void LoadSoundAsync(const TSoftObjectPtr<USoundBase>& Sound, TFunction<void(USoundBase*)> OnLoaded);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Mix")
	TObjectPtr<USoundMix> GlobalMix = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Class")
	TObjectPtr<USoundClass> MasterClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Class")
	TObjectPtr<USoundClass> BGMClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Class")
	TObjectPtr<USoundClass> SFXClass = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> BGMComponent = nullptr;

	//TSharedPtr<FStreamableHandle> PendingBGMHandle;
	FSoundOptionData OptionData;
};
