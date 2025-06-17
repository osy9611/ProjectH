// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTurnSlot.h"
#include "Components/Image.h"
#include "PaperSprite.h"
UBattleTurnSlot::UBattleTurnSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UBattleTurnSlot::OnInit(UPaperSprite* PaperSprite)
{
	if (!PaperSprite)
		return;

	const FSlateAtlasData SpriteAtlasData = PaperSprite->GetSlateAtlasData();
	const FVector2D SpriteSize = SpriteAtlasData.GetSourceDimensions();

	FSlateBrush Brush;
	Brush.SetResourceObject(PaperSprite);

	float Width = 0;
	float Height = 0;

	Width = (Width > 0) ? Width : SpriteSize.X;
	Height = (Height > 0) ? Height : SpriteSize.Y;
	Brush.ImageSize = FVector2D(Width, Height);
	CharImage->SetBrush(Brush);
}


