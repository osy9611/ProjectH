// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Notifies/PaperZDAnimNotify.h"
#include "PaperZDAnimNotify_Damage.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTH_API UPaperZDAnimNotify_Damage : public UPaperZDAnimNotify
{
	GENERATED_BODY()
public:
	UPaperZDAnimNotify_Damage();
	void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance = nullptr) const override;
public:
	TFunction<void()> OnCallback;
};
