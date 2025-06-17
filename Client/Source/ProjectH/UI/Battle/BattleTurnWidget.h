// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BattleTurnWidget.generated.h"

class UHorizontalBox;
class UBattleTurnSlot;
class UTurnManager;
class UHDBattleComponent;
UCLASS()
class PROJECTH_API UBattleTurnWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UBattleTurnWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void OnInit();
	
	void GetTurnSlots();
	void CheckCreateSlot();

	void UpdateSlots();

	void UpdateSlot_Char(int32 Index, UHDBattleComponent* BattleComp);
	void UpdateSlot_Monster(int32 Index, UHDBattleComponent* BattleComp);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> TurnSlotRoot;

private:
	UPROPERTY()
	UBattleTurnSlot* SelectSlot;

	UPROPERTY()
	TArray<UBattleTurnSlot*> TurnSlots;

	TWeakObjectPtr<UTurnManager> TurnManager;
};
