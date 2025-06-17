// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "HDCharacter.generated.h"

class UModularCameraComponent;
class UHDPawnExtensionComponent;

//�ش� ������Ʈ������ MoudularChar�� ������� �ʴ´� (2D ĳ���͸� ����ϱ� ����)
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
