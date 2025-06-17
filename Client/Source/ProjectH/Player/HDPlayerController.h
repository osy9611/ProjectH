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

	//���������� �÷��̾� ��Ʈ�ѷ��� ĳ���Ϳ��ٰ� ��������Ʈ�� ���ε��ϴµ� �װ� �÷��̾� ��Ʈ�ѷ��� ĳ���Ϳ��� �Ѱ��ش�
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	UHDAbilitySystemComponent* GetHDAbilitySystemComponent() const;
	AHDPlayerState* GetPTPlayerState() const;
};
