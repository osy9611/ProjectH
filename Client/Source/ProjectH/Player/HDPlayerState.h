// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ProjectH/AbilitySystem/HDAbilitySystemComponent.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"
#include "HDPlayerState.generated.h"

class UHDPawnData;
class UHDExperienceDefinition;
class UHDAbilitySystemComponent;
class UHDAttributeSet;
class UHDAbilitySet;

UCLASS()
class PROJECTH_API AHDPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AHDPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() final;

	template<class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void OnExperienceLoaded(const UHDExperienceDefinition* CurrentExperience);
	void SetPawnData(const UHDPawnData* InPawnData);
	void RegisterAbilitySet(const TArray<TObjectPtr<UHDAbilitySet>>& AbilitySets);
	UHDAbilitySystemComponent* GetHDAbilitySystemComponent() const { return AbilitySystemComponent; }
	UHDAttributeSet* GetHDAttributeSet() const { return AttributeSet; }


	/*
	* CreateDefaultSubobject�� ������ ������ ����ϱ� ������
	* NewObject�� �����
	*/
	template<class T = UHDAttributeSet>
	T* CreateAttribute()
	{
		if (!HasAnyFlags(RF_ClassDefaultObject))
		{
			AttributeSet = NewObject<T>(this, T::StaticClass(), TEXT("AttributeSet"));
			AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet.Get());
		}
		return Cast<T>(AttributeSet);
	}

	void RegisterAttributeSet(UHDAttributeSet* HDAttributeSet)
	{
		if (!HasAnyFlags(RF_ClassDefaultObject))
		{
			AttributeSet = HDAttributeSet;
			AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet.Get());
		}
	}
private:
	UPROPERTY()
	TObjectPtr<const UHDPawnData> PawnData;

	/*
	 * �����Ƽ ������Ʈ�� ���⿡ �ִ� ����
	 *  - ��Ʈ��ũ�� ����ϸ� �̰��� �߰��� �ؾ��Ѵ�.
	 *  - PlayerState�� �ֱ������� �������� Ŭ���̾�Ʈ�� �����Ǵ� �����̱� ����
	*/
	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
	TObjectPtr<UHDAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
	TObjectPtr<UHDAttributeSet> AttributeSet;
};
