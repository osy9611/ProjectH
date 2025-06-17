// Fill out your copyright notice in the Description page of Project Settings.


#include "HDPawnExtensionComponent.h"
#include "PaperZDAnimInstance.h"
#include "PaperZDAnimationComponent.h"
#include "PaperZDCharacter.h"
#include "PaperFlipbook.h"     
#include "PaperSprite.h"  
#include "HDPawnData.h"   
#include "PaperFlipbookComponent.h"
#include "GameFramework/Character.h"
#include "Components/GameFrameworkComponentManager.h"
#include "ProjectH/LogChannels.h"
#include "ProjectH/HDGameplayTags.h"
#include "ProjectH/Util/UtilFunc_Sprite.h"
#include "ProjectH/System/HDAssetManager.h"
#include "ProjectH/AbilitySystem/HDAbilitySystemComponent.h"

/*feature name을 component 단위니깐 component를 빼고 pawn extension만 넣는 것은 확인할 수 있다.*/
const FName UHDPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UHDPawnExtensionComponent::UHDPawnExtensionComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UHDPawnExtensionComponent::SetPawnData(const UHDPawnData* InPawnData)
{
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
		return;
	if (PawnData)
		return;

	PawnData = InPawnData;
}

void UHDPawnExtensionComponent::SetPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UHDPawnExtensionComponent::SpawnDefaultController()
{
	APawn* Pawn = GetPawnChecked<APawn>();
	Pawn->SpawnDefaultController();
}

void UHDPawnExtensionComponent::InitializeGameAbilitySystem(AActor* IsOwnerActor)
{
	if (!IsOwnerActor)
		return;

	UHDAbilitySystemComponent* ASC = CreateDefaultSubobject<UHDAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	RegisterGameAbilitySystem(ASC, IsOwnerActor);
}

void UHDPawnExtensionComponent::RegisterGameAbilitySystem(UHDAbilitySystemComponent* InASC, AActor* IsOwnerActor)
{
	check(InASC && IsOwnerActor);

	if (AbilitySystemComponent == InASC)
		return;

	if (AbilitySystemComponent)
		UnRegisterGameAbilitySystem();

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingActor = InASC->GetAvatarActor();
	check(!ExistingActor);

	//ASC를 업데이트하고, InitAbilityActorInfo를 Pawn과 같이 호출하여, AvatarActor를 Pawn으로 업데이트 해준다
	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(IsOwnerActor, Pawn);
}

void UHDPawnExtensionComponent::UnRegisterGameAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (AbilitySystemComponent)
		AbilitySystemComponent = nullptr;
}

void UHDPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	//올바른 Actor에 등록되었는지 확인
	//OnRegister 단계에서 컴포넌트를 부착한다. 만약에 부착에 실패했다는 거는 컴포넌트 자체가 없기 때문
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(HDLog, Error, TEXT("this component has been added to a BP whose base class is not a Pawn"));
			return;
		}
	}

	/*
	* GameFrameworkComponentManager에 InitState 사용을 위해 등록 진행
	*  - 등록은 상속받았던 IGameFrameworkInitStateInterface 매서드 RegisterInitStateFeature()를 활용
	*  - 해당 함수를 간단히 보기
	* GameInstance에서 정의한 InitState 정보를 사용하기 위해서 등록하기 위한 함수로만 생각하자
	*/
	RegisterInitStateFeature();
}

void UHDPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	//InitState_Spawned로 상태 변환
	// - TryToChangeInitState는 다음과 같이 진행된다.
	//	 1.CanChangeInitState로 상태 변환 가능성 유무 판단
	//	 2.HandleChangeInitState로 내부 상태 변경
	//	 3.BindOnActorInitStateChanged로 Bind된 Delegate를 조건에 맞게 호출해 줌
	//     - PTPawnExtensionComponent의 경우, 모든 Actor의 Feature 상태 변화에 대해 OnActorInitStateChanged()가 호출됨

	ensure(TryToChangeInitState(FHDGameplayTags::Get().InitState_Spawned));
}

void UHDPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UHDPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		const FHDGameplayTags& InitTags = FHDGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataAvailable)
			CheckDefaultInitialization();
	}
}

bool UHDPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();

	//InitState_Spawned 초기화
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
			return true;
	}

	//Spawned -> DataAvailable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		//PawnData를 설정하고 있을 것이다
		if (!PawnData)
			return false;

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		if (bIsLocallyControlled)
		{
			if (!GetController<AController>())
				return false;
		}

		return true;
	}

	//DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		//Actor에 바인드된 모든 Feature들이 DataAvailable 상태일 때, DataInitialized로 넘어감:
		// - HaveAllFeaturesReachedInitState 확인
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void UHDPawnExtensionComponent::CheckDefaultInitialization()
{
	CheckDefaultInitializationForImplementers();

	const FHDGameplayTags& InitTags = FHDGameplayTags::Get();

	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	ContinueInitStateChain(StateChain);
}

void UHDPawnExtensionComponent::SetFlipbook(const FString Path)
{
	if (Path.IsEmpty())
		return;
	UHDAssetManager& AssetManager = UHDAssetManager::Get();

	UObject* Result = AssetManager.SynchronusLoadAsset(Path);
	if (UPaperFlipbook* PaperFlipbook = Cast<UPaperFlipbook>(Result))
	{
		SetFlipbook(PaperFlipbook);
	}
}

void UHDPawnExtensionComponent::SetFlipbook(UPaperFlipbook* NewFlipbook)
{
	if (!NewFlipbook)
	{
		UE_LOG(HDLog, Log, TEXT("[HDPawnExtensionComponent] SetSprite Fail Flipbook is nullptr"));
		return;
	}
	APawn* Pawn = GetPawn<APawn>();
	UPaperFlipbookComponent* FlipbookComponent = UtilFunc_Sprite::GetFlipbookComponent(Pawn);
	bool Result = FlipbookComponent->SetFlipbook(NewFlipbook);
	FlipbookComponent->Play();
}

void UHDPawnExtensionComponent::SetAnimationInstance(const FString Path)
{
	UHDAssetManager& AssetManager = UHDAssetManager::Get();

	//TSubclassOf를 위해서 _C를 붙여야함
	FString ResultPath = Path;
	int32 DotIndex;
	if (Path.FindLastChar('.', DotIndex))
	{
		ResultPath = Path + TEXT("_C");
	}

	UObject* Result = AssetManager.SynchronusLoadAsset(ResultPath);

	UClass* AnimClass = Cast<UClass>(Result);
	if (AnimClass && AnimClass->IsChildOf(UPaperZDAnimInstance::StaticClass()))
	{
		TSubclassOf<UPaperZDAnimInstance> Subclass = AnimClass;
		SetAnimationInstance(Subclass);
	}
}

void UHDPawnExtensionComponent::SetAnimationInstance(TSubclassOf<UPaperZDAnimInstance> AnimInstance)
{
	APawn* Pawn = GetPawn<APawn>();

	APaperZDCharacter* PaperCharacter = Cast<APaperZDCharacter>(Pawn);
	if (!PaperCharacter)
		return;

	UPaperZDAnimationComponent* AnimComponent = PaperCharacter->GetAnimationComponent();

	if (AnimComponent)
	{
		AnimComponent->InitAnimInstanceClass(AnimInstance);
		AnimComponent->GetOrCreateAnimInstance();
	}

	UPaperFlipbookComponent* FlipbookComponent = PaperCharacter->GetSprite();
	FlipbookComponent->Play();
}
void UHDPawnExtensionComponent::SetFlipX(double dirX)
{
	if (FMath::Abs(dirX) > KINDA_SMALL_NUMBER)
	{
		bool NewFacingRight = dirX > 0.0f;


		if (NewFacingRight != FacingRight)
		{
			FacingRight = NewFacingRight;

			APawn* Pawn = GetPawn<APawn>();
			APaperZDCharacter* PaperCharacter = Cast<APaperZDCharacter>(Pawn);
			if (!PaperCharacter)
				return;
			UPaperFlipbookComponent* FlipbookComponent = PaperCharacter->GetSprite();
			FVector Scale = FlipbookComponent->GetRelativeScale3D();
			Scale.X = FMath::Abs(Scale.X) * (FacingRight ? -1.f : 1.f);
			FlipbookComponent->SetRelativeScale3D(Scale);
		}
	}
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS