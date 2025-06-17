// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "HDPlayerController.generated.h"

class AHDPlayerState;
class UHDAbilitySystemComponent;

UCLASS()
class PROJECTH_API AHDPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
public:
	AHDPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//내부적으로 플레이어 컨트롤러가 캐릭터에다가 델리게이트를 바인드하는데 그게 플레이어 컨트롤러를 캐릭터에게 넘겨준다
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	UHDAbilitySystemComponent* GetHDAbilitySystemComponent() const;
	AHDPlayerState* GetPTPlayerState() const;
};
