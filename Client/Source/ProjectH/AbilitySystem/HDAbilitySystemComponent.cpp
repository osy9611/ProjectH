// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAbilitySystemComponent.h"
#include "ProjectH/Util/UtilFunc_Data.h"
#include "ProjectH/AbilitySystem/GameEffect/HDGE_Buff.h"
#include "ProjectH/AbilitySystem/GameEffect/HDGE_DeBuff.h"
#include "ProjectH/AbilitySystem/GameEffect/HDGE_Damage.h"
#include "ProjectH/AbilitySystem/AttributeSet/HDAttributeSet.h"

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
	for (TPair<FActiveGameplayEffectHandle, int32>& BuffTurn : BuffTurns)
	{
		FActiveGameplayEffectHandle& Handle = BuffTurn.Key;

		const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect(Handle);
		const FGameplayEffectContextHandle& ContextHandle = ActiveEffect->Spec.GetEffectContext();
		if (!ContextHandle.IsValid())
			continue;
		FBuffEffectContext* BuffContext = (FBuffEffectContext*)ContextHandle.Get();
		if (!BuffContext)
			continue;

		if (BuffIDs.Contains(BuffContext->BuffID))
		{
			if (BuffContext->Data.Stackable)
			{
				ApplyGameplayEffectSpecToSelf(ActiveEffect->Spec);
				BuffIDs.Remove(BuffContext->BuffID);
				ResetBuffHandle(Handle, BuffContext->Data.TurnCount);
			}
		}
	}

	for (int32 BuffID : BuffIDs)
	{
		FBuffData* Data = UtilFunc_Data::GetTableData<FBuffData>(GetWorld(), FString::FromInt(BuffID));
		if (!Data)
		{
			UE_LOG(HDLog, Log, TEXT("[UHDAbilitySystemComponent] BuffData Not Found"));
			continue;
		}

		FBuffEffectContext* BuffContext = new FBuffEffectContext();
		BuffContext->AddInstigator(Source, Source);
		BuffContext->BuffID = BuffID;
		BuffContext->Data = *Data;

		FActiveGameplayEffectHandle Handle = ApplyGenericEffect(UHDGE_Buff::StaticClass(), BuffContext);
		if (Handle.IsValid())
		{
			AddBuffHandle(Handle, Data->TurnCount);
		}
	}
}

void UHDAbilitySystemComponent::RegisterDebuff(TArray<int32> BuffIDs, AActor* Source)
{
	for (TPair<FActiveGameplayEffectHandle, int32>& DeBuffTurn : DeBuffTurns)
	{
		FActiveGameplayEffectHandle& Handle = DeBuffTurn.Key;

		const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect(Handle);
		const FGameplayEffectContextHandle& ContextHandle = ActiveEffect->Spec.GetEffectContext();
		if (!ContextHandle.IsValid())
			continue;
		FDeBuffEffectContext* DeBuffContext = (FDeBuffEffectContext*)ContextHandle.Get();
		if (!DeBuffContext)
			continue;

		if (BuffIDs.Contains(DeBuffContext->DeBuffID))
		{
			if (DeBuffContext->Data.Stackable)
			{
				ApplyGameplayEffectSpecToSelf(ActiveEffect->Spec);
				BuffIDs.Remove(DeBuffContext->DeBuffID);
				ResetBuffHandle(Handle, DeBuffContext->Data.TurnCount);
			}
		}
	}

	for (int32 BuffID : BuffIDs)
	{
		FDeBuffData* Data = UtilFunc_Data::GetTableData<FDeBuffData>(GetWorld(), FString::FromInt(BuffID));
		if (!Data)
		{
			UE_LOG(HDLog, Log, TEXT("[UHDAbilitySystemComponent] BuffData Not Found"));
			continue;
		}

		FDeBuffEffectContext* DeBuffContext = new FDeBuffEffectContext();
		DeBuffContext->AddInstigator(Source, Source);
		DeBuffContext->DeBuffID = BuffID;
		DeBuffContext->Data = *Data;

		FActiveGameplayEffectHandle Handle = ApplyGenericEffect(UHDGE_DeBuff::StaticClass(), DeBuffContext);
		if (Handle.IsValid())
		{
			AddDeBuffHandle(Handle, Data->TurnCount);
		}
	}
}


FActiveGameplayEffectHandle UHDAbilitySystemComponent::ApplyGenericEffect(TSubclassOf<UGameplayEffect> GEClass, FGameplayEffectContext* Context)
{
	if (!IsValid(GEClass))
	{
		UE_LOG(HDLog, Warning, TEXT("[UHDAbilitySystemComponent] GEClass Is Not Valid"));
		return FActiveGameplayEffectHandle();
	}

	FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle(Context);
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(GEClass, 1.0f, ContextHandle);

	if (SpecHandle.IsValid())
	{
		return ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());		
	}

	return FActiveGameplayEffectHandle();
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

void UHDAbilitySystemComponent::AddDeBuffHandle(FActiveGameplayEffectHandle& Handle, int32 TurnCount)
{
	if (!DeBuffTurns.Contains(Handle))
		DeBuffTurns.Add(Handle, TurnCount);
}

void UHDAbilitySystemComponent::ResetDeBuffHandle(FActiveGameplayEffectHandle& Handle, int32 TurnCount)
{
	if (DeBuffTurns.Contains(Handle))
		DeBuffTurns[Handle] = TurnCount;
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
		const UHDAttributeSet* SourceAttributeSet = GetSet<UHDAttributeSet>();
		RemoveActiveGameplayEffect(Handle);
		BuffTurns.Remove(Handle);
	}
}

void UHDAbilitySystemComponent::UpdateDeBuff()
{
	TArray<FActiveGameplayEffectHandle> RemoveBuffHandles;
	for (TPair<FActiveGameplayEffectHandle, int32>& DeBuffTurn : DeBuffTurns)
	{
		DeBuffTurn.Value--;

		if (DeBuffTurn.Value <= 0)
		{
			RemoveBuffHandles.Add(DeBuffTurn.Key);
			continue;
		}

		const FActiveGameplayEffect* ActiveGE = GetActiveGameplayEffect(DeBuffTurn.Key);

		if (!ActiveGE)
			continue;

		ExecutePeriodicEffect(DeBuffTurn.Key);
	}

	for (FActiveGameplayEffectHandle& Handle : RemoveBuffHandles)
	{
		RemoveActiveGameplayEffect(Handle);
		DeBuffTurns.Remove(Handle);
	}
}