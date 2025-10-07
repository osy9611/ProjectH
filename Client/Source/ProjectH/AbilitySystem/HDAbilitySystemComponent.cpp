// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAbilitySystemComponent.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/AbilitySystem/GameEffect/HDGE_Buff.h"

UHDAbilitySystemComponent::UHDAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
{
}
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void UHDAbilitySystemComponent::ProcessAbilityAndParam(const FGameplayTag& Tag, const FGameAbilityParam& Params)
{
	if (Tag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && !AbilitySpec.IsActive() && (AbilitySpec.DynamicAbilityTags.HasTagExact(Tag)))
			{
				StoreParamsForAbility(AbilitySpec.Handle, Params);
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS
void UHDAbilitySystemComponent::StoreParamsForAbility(FGameplayAbilitySpecHandle Handle, const FGameAbilityParam& Params)
{
	if (PendingParams.Contains(Handle))
	{
		PendingParams[Handle] = Params.Clone();
		return;
	}

	PendingParams.Add(Handle, Params.Clone());
}

FGameAbilityParam* UHDAbilitySystemComponent::ConsumeParams(FGameplayAbilitySpecHandle Handle)
{
	FGameAbilityParam** Params = PendingParams.Find(Handle);
	if (!Params)
		return nullptr;
	PendingParams.Remove(Handle);
	return *Params;
}

void UHDAbilitySystemComponent::RegisterBuff(TArray<int32> BuffIDs, AActor* Source)
{
	const FGameplayEffectQuery Query;
	TArray<FActiveGameplayEffectHandle> BuffEffectHandles = GetActiveEffects(Query);

	for (FActiveGameplayEffectHandle& Handle : BuffEffectHandles)
	{
		const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect(Handle);
		const FGameplayEffectContextHandle& ContextHandle = ActiveEffect->Spec.GetEffectContext();
		if (!ContextHandle.IsValid())
			continue;
		FBuffEffectContext* BuffContext = (FBuffEffectContext*)ContextHandle.Get();
		if (!BuffContext)
			continue;

		if (BuffIDs.Contains(BuffContext->BuffID))
		{
			const UGameplayEffect* GEClass = ActiveEffect->Spec.Def;
			if (BuffContext->BuffData.Stackable)
			{
				ApplyGameplayEffectSpecToSelf(ActiveEffect->Spec);
				BuffIDs.Remove(BuffContext->BuffID);
				ResetBuffHandle(Handle, BuffContext->BuffData.TurnCount);
			}
		}
	}

	AActor* SourceActor = GetAvatarActor();
	for (int32 BuffID : BuffIDs)
	{
		FBuffData* Data = UtilFunc_Data::GetTableData<FBuffData>(GetWorld(), FString::FromInt(BuffID));
		if (!Data)
		{
			UE_LOG(HDLog, Log, TEXT("[UHDGameplayAbility_BuffSkill] BuffData Not Found"));
			continue;
		}

		FBuffEffectContext* BuffContext = new FBuffEffectContext();
		BuffContext->AddInstigator(Source, Source);
		BuffContext->BuffID = BuffID;
		BuffContext->BuffData = *Data;

		TSubclassOf<UGameplayEffect> GEClass = UHDGE_Buff::StaticClass();
		FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle(BuffContext);
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(GEClass, 1.0f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle Handle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (Handle.IsValid())
			{
				AddBuffHandle(Handle, Data->TurnCount);
			}
		}
	}
}

void UHDAbilitySystemComponent::RegisterDebuff(TArray<int32> BuffIDs, AActor* Source)
{
}

void UHDAbilitySystemComponent::AddBuffHandle(FActiveGameplayEffectHandle& Handle, int32 TurnCount)
{
	if (!BuffTurns.Contains(Handle))
		BuffTurns.Add(Handle, TurnCount);
}

void UHDAbilitySystemComponent::ResetBuffHandle(FActiveGameplayEffectHandle& Handle, int32 TurnCount)
{
	if (BuffTurns.Contains(Handle))
		BuffTurns[Handle] = TurnCount;
}

void UHDAbilitySystemComponent::UpdateGE()
{
	UpdateBuff();
	UpdateDeBuff();
}

void UHDAbilitySystemComponent::UpdateBuff()
{
	TArray<FActiveGameplayEffectHandle> RemoveBuffHandles;
	for (TPair<FActiveGameplayEffectHandle, int32>& BuffTurn : BuffTurns)
	{
		BuffTurn.Value--;

		if (BuffTurn.Value <= 0)
		{
			RemoveBuffHandles.Add(BuffTurn.Key);
			continue;
		}
	}

	for (FActiveGameplayEffectHandle& Handle : RemoveBuffHandles)
	{
		RemoveActiveGameplayEffect(Handle);
		BuffTurns.Remove(Handle);
	}
}

void UHDAbilitySystemComponent::UpdateDeBuff()
{
	TArray<FActiveGameplayEffectHandle> RemoveBuffHandles;
	for (TPair<FActiveGameplayEffectHandle, int32>& BuffTurn : BuffTurns)
	{
		BuffTurn.Value--;

		if (BuffTurn.Value <= 0)
		{
			RemoveBuffHandles.Add(BuffTurn.Key);
			continue;
		}
	}

	for (FActiveGameplayEffectHandle& Handle : RemoveBuffHandles)
	{
		RemoveActiveGameplayEffect(Handle);
		BuffTurns.Remove(Handle);
	}
}