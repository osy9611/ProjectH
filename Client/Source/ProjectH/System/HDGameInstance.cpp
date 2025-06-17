// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameInstance.h"
#include "ProjectH/HDGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "ProjectH/Input/HDInputManagerSubsystem.h"
#include "ProjectH/Battle/BattleSubsystem.h"
#include "CommonLocalPlayer.h"

void UHDGameInstance::Init()
{
	Super::Init();

	//앞서 정의한 InitState의 GaemplayTags 등록
	//UGameFrameworkComponentManager : Component를 관리하는 매니져
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
	if (ensure(ComponentManager))
	{
		const FHDGameplayTags& GameplayTags = FHDGameplayTags::Get();
		ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_DataAvailable);
		ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
	}
}

void UHDGameInstance::Shutdown()
{
	Super::Shutdown();
}

int32 UHDGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, UserId);
	if (ReturnVal != INDEX_NONE)
	{
		UCommonLocalPlayer* CommonLocalPlayer = Cast<UCommonLocalPlayer>(NewPlayer);
		GetSubsystem<UHDInputManagerSubsystem>()->NotifyPlayerAdded(Cast<UCommonLocalPlayer>(CommonLocalPlayer));

		UBattleSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleSubsystem>();
		if (BattleSubsystem->CheckBattleMode())
			GetWorld()->GetSubsystem<UBattleSubsystem>()->InitInput();
	}
	return int32();
}

bool UHDGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	UCommonLocalPlayer* CommonLocalPlayer = Cast<UCommonLocalPlayer>(ExistingPlayer);
	GetSubsystem<UHDInputManagerSubsystem>()->NotifyPlayerDestroyed((CommonLocalPlayer));
	return Super::RemoveLocalPlayer(ExistingPlayer);
}
