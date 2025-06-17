// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "HDCharacter.generated.h"

class UModularCameraComponent;
class UHDPawnExtensionComponent;

//해당 프로젝트에서는 MoudularChar를 사용하지 않는다 (2D 캐릭터를 써야하기 때문)
UCLASS()
class PROJECTH_API AHDCharacter : public APaperZDCharacter
{
	GENERATED_BODY()
public:
	AHDCharacter();

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReson) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UHDPawnExtensionComponent> PawnExtComponent;
};
