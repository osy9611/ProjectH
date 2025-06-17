// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "UIManagerSubsystem.generated.h"

class UCommonLocalPlayer;
class UCommonUserWidget;
class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;

UCLASS()
class COMMONGAME_API UUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UUIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual void NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer);
	virtual void NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer);
	virtual void NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer);

	void RemoveLayoutFromViewport();

	UCommonActivatableWidgetContainerBase* GetLayerContainer(FGameplayTag LayerName) { return Layers.FindRef(LayerName); }

	UFUNCTION(BlueprintCallable)
	void RegisterWidgetContainer(FGameplayTag LayerName, UCommonActivatableWidgetContainerBase* Container);

	UFUNCTION(BlueprintCallable)
	virtual UCommonActivatableWidget* CreateUI(FGameplayTag LayerTag, TSubclassOf<UCommonActivatableWidget> WidgetClass);
	UFUNCTION(BlueprintCallable)
	virtual UCommonActivatableWidget* FindWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass);
private:
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;

	UPROPERTY()
	TMap<TSubclassOf<UCommonActivatableWidget>, TObjectPtr<UCommonActivatableWidget>> CacheWidgets;

};
