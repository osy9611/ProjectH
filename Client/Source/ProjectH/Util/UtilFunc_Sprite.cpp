// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilFunc_Sprite.h"
#include "PaperSprite.h"  
#include "PaperFlipbook.h"     
#include "PaperFlipbookComponent.h"
#include "PaperZDCharacter.h"
#include "PaperZDAnimInstance.h"
#include "PaperZDAnimationComponent.h"
#include "ProjectH/Character/HDPawnExtensionComponent.h"
#include "ProjectH/LogChannels.h"

UPaperFlipbookComponent* UtilFunc_Sprite::GetFlipbookComponent(AActor* Actor)
{
	if (!IsValid(Actor))
		return nullptr;

	APaperZDCharacter* PaperCharacter = Cast<APaperZDCharacter>(Actor);
	if (!PaperCharacter)
		return nullptr;

	UPaperFlipbookComponent* FlipbookComp = PaperCharacter->GetSprite();
	if (!FlipbookComp)
		return nullptr;

	return FlipbookComp;
}

UPaperSprite* UtilFunc_Sprite::GetSprite(AActor* Actor)
{
	if (!IsValid(Actor))
		return nullptr;

	UPaperFlipbookComponent* FlipbookComp = GetFlipbookComponent(Actor);
	if (!FlipbookComp)
		return nullptr;

	UPaperFlipbook* Flipbook = FlipbookComp->GetFlipbook();
	if (!Flipbook)
		return nullptr;

	const int32 FrameIndex = FlipbookComp->GetPlaybackPositionInFrames();

	return FlipbookComp->GetFlipbook()->GetSpriteAtFrame(FrameIndex);
}

FVector UtilFunc_Sprite::GetSpriteSize(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		UE_LOG(HDLog, Log, TEXT("[UtilFunc_Sprite] GetSpriteSize Fail Actor is nullptr"));
		return FVector();
	}

	UPaperSprite* Sprite = GetSprite(Actor);

	if (!Sprite)
	{
		UE_LOG(HDLog, Log, TEXT("[UtilFunc_Sprite] GetSpriteSize Fail Sprite is nullptr"));
		return FVector();
	}

	FBoxSphereBounds RenderBounds = Sprite->GetRenderBounds();
	FVector Extent = RenderBounds.BoxExtent;

	return Extent;
}