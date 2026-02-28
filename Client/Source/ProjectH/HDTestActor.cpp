// Fill out your copyright notice in the Description page of Project Settings.


#include "HDTestActor.h"

// Sets default values
AHDTestActor::AHDTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AHDTestActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    static const FName EffectTypeName = GET_MEMBER_NAME_CHECKED(AHDTestActor, EffectType);

    if (PropertyChangedEvent.GetPropertyName() == EffectTypeName)
    {
        bEnableFirePower = (EffectType == EEffectType::Fire);
        bEnableIcePower = (EffectType == EEffectType::Ice);
    }
}
