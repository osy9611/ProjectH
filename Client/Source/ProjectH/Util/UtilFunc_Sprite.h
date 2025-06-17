// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UPaperFlipbookComponent;
class UPaperSprite;

class PROJECTH_API UtilFunc_Sprite
{
public:
	static UPaperFlipbookComponent* GetFlipbookComponent(AActor* Actor);
	static UPaperSprite* GetSprite(AActor* Actor);
	static FVector GetSpriteSize(AActor* Actor);
};
