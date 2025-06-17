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
	* CreateDefaultSubobject는 생성자 내에서 사용하기 때문에
	* NewObject로 써야함
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
	 * 어빌리티 컴포넌트가 여기에 있는 이유
	 *  - 네트워크를 고려하면 이곳에 추가를 해야한다.
	 *  - PlayerState는 주기적으로 서버에서 클라이언트로 배포되는 엑터이기 때문
	*/
	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
	TObjectPtr<UHDAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
	TObjectPtr<UHDAttributeSet> AttributeSet;
};
