// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "ProjectH/Data/EnumGenerateData.h"
#include "HDBattleComponent.generated.h"

class UHDAttributeSet;
struct FBattleStateParams;

UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class PROJECTH_API UHDBattleComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UHDBattleComponent(const FObjectInitializer& ObjectInitializer);

	static UHDBattleComponent* FindBattleComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHDBattleComponent>() : nullptr); }

	static const FName NAME_ActorFeatureName;

	virtual void OnRegister() final;	//컴포넌트가 생성되는 완전 초기에 호출되는 함수
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; } //특정 Feature를 가진 엑터로 처리하도록 식별하는 중요한 부분
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) final;
	virtual void CheckDefaultInitialization() final;

	void ProcessAbility(FGameplayTag Tag);
	void ProcessAbility_Skill(const FBattleStateParams& Params);
	void RegisterBattleData(UHDAttributeSet* AttributeSet);

	UHDAttributeSet* GetAttributeSet() const;
	bool CheckDead();

	void ChachedWidgets(FString WidgetName, TFunction<void(UWidgetComponent*)> Result);

	UWidgetComponent* GetWidgetComponent(FString WidgetName);

	template<typename T = UUserWidget>
	T* GetWidgetObject(FString WidgetName, bool VisibleComp);


public:
	FString TableID;
	int32 SlotNo;
	ECharType CharType;

private:
	TMap<FString, UWidgetComponent*> ChachedWidgetComps;
};

template<typename T>
T* UHDBattleComponent::GetWidgetObject(FString WidgetName, bool VisibleComp)
{
	UWidgetComponent* WidgetComp = GetWidgetComponent(WidgetName);
	if (!WidgetComp)
		return nullptr;

	UUserWidget* UserWidget = WidgetComp->GetUserWidgetObject();
	if (!UserWidget)
		return nullptr;

	WidgetComp->SetVisibility(VisibleComp);
	return Cast<T>(UserWidget);
}
