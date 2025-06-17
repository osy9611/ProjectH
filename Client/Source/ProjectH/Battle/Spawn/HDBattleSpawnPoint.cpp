// Fill out your copyright notice in the Description page of Project Settings.


#include "HDBattleSpawnPoint.h"

AHDBattleSpawnPoint::AHDBattleSpawnPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}
