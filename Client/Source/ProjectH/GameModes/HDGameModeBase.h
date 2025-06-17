// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HDGameModeBase.generated.h"

class UHDPawnData;
class UHDExperienceDefinition;

UCLASS()
class PROJECTH_API AHDGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AHDGameModeBase();

	//게임 극 초반 시점
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) final;

	/*
	* HandleStartingNewPlayer
	*  - 새로운 플레이어가 게임에 접속할 때 초기 설정을 수행하는 함수
	*  - 플레이어가 게임에 참가한 즉시 기본 폰을 생성하고, 스폰 위치에 배치하면 초기화 작업을 수행하는 중요한 역할을 한다.
	*  - Unity의 OnSceneLoaded와 유사함
	*/
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;

	/*
	* SpawnDefaultPawnAtTransform
	* 게임에 스폰될 때 사용할 기본 폰을 생성하는 함수. 주로 플레이어가 레벨에 처음 진입할 떄 호출되며, 기본 폰을 특정 위치와 회전값을 가지는 변환에
	* 생성하기 위해서 사용된다.
	*/
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;

	/*
	* ChoosePlayerStart_Implementation
	* 게임에 스폰될 때 어느 지점에서 시작할지를 결정하기 위해 사용된다.
	*/
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) final;

	void HandleMatchAssignmentIfNotExceptingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId,FString SceneTableNo);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const UHDExperienceDefinition* CurrentExperience);
	const UHDPawnData* GetPawnDataForController(const AController* InController) const;
};
