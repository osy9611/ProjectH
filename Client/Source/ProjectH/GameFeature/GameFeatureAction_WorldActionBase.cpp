// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction_WorldActionBase.h"

void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			//WorldActionBase의 Interface인 AddWorld 호출
			AddToWorld(WorldContext, Context);
		}
	}
}
