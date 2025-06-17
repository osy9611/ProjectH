// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAttributeSet.h"

UHDAttributeSet::UHDAttributeSet(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UHDAttributeSet::OnInit(FString InitTableID)
{
	TableID = InitTableID;
}

void UHDAttributeSet::OnUpdateStatus()
{
	
}
