// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularCameraMode.h"
#include "ModularPlayerCameraManager.h"
#include "ModularCameraComponent.h"

FCameraModeView::FCameraModeView()
	:Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(PT_CAMERA_DEFAULT_FOV)
{
}

void FCameraModeView::Blend(const FCameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.0f)
		return;
	else if (OtherWeight >= 1.0f)
	{
		//Weight가 1.0이면 Other를 덮어쓰면 된다.
		*this = Other;
		return;
	}

	//Location + OtherWeight * (Other.Location - Location)
	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	//Location과 같은 방식 Lerp (ControlRotation과 FieldOfView도 같음)
	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

UModularCameraMode::UModularCameraMode(const FObjectInitializer& ObjectInitializer)
{
	FieldOfView = PT_CAMERA_DEFAULT_FOV;
	ViewPitchMin = PT_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = PT_CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.5f;
	BlendFunction = ECameraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
}

void UModularCameraMode::UpdateCameraMode(float DeltaTime)
{
	//Actor를 활용하여, Pivot[Location|Rotation]을 계산하여, View를 업데이트
	UpdateView(DeltaTime);

	//BlendWeight를 DeltaTime을 활용하여, BlendAlpha 계산 후, BlendFunction에 맞게 재 매핑하여 최종 계산
	UpdateBlending(DeltaTime);
}

void UModularCameraMode::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;
}

void UModularCameraMode::UpdateBlending(float DeltaTime)
{//BlendAlpha를 DeltaTime을 통해 계산
	if (BlendTime > 0.0f)
	{
		BlendAlpha += (DeltaTime / BlendTime);
		BlendAlpha = FMath::Min(BlendAlpha, 1.0f);
	}
	else
		BlendAlpha = 1.0f;

	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;
	switch (BlendFunction)
	{
	case ECameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;
	case ECameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case ECameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case ECameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	default:
		checkf(false, TEXT("UpdateBlending: Ivalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

UModularCameraComponent* UModularCameraMode::GetCameraComponent() const
{
	//UPTCameraModeStack::GetCameraModeInstacne()확인
	return CastChecked<UModularCameraComponent>(GetOuter());
}

AActor* UModularCameraMode::GetTargetActor() const
{
	const UModularCameraComponent* PTCameraComponent = GetCameraComponent();
	return PTCameraComponent->GetTargetActor();
}

FVector UModularCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	//BaseEyeHeight를 고려하여, ViewLocation을 반환
	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
		return TargetPawn->GetPawnViewLocation();

	return TargetActor->GetActorLocation();
}

FVector UModularCameraMode::GetPivotForward() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	return TargetActor->GetActorForwardVector();
}

FRotator UModularCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	//GetViewRotation() 확인, 보통 Pawn의 ControlRotation을 반환
	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
		return TargetPawn->GetViewRotation();

	return TargetActor->GetActorRotation();
}

void UModularCameraMode::SetBlendWeight(float Weight)
{
	BlendWeight = FMath::Clamp(Weight, 0.0f, 1.0f);

	const float InvExponent = (BlendExponent > 0.0f) ? (1.0f / BlendExponent) : 1.0f;

	switch (BlendFunction)
	{
	case ECameraModeBlendFunction::Linear:
		BlendAlpha = BlendWeight;
		break;

	case ECameraModeBlendFunction::EaseIn:
		BlendAlpha = FMath::InterpEaseIn(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	case ECameraModeBlendFunction::EaseOut:
		BlendAlpha = FMath::InterpEaseOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	case ECameraModeBlendFunction::EaseInOut:
		BlendAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	default:
		checkf(false, TEXT("SetBlendWeight: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

UCameraModeStack::UCameraModeStack(const FObjectInitializer& ObjectInitializer)
{
}

UModularCameraMode* UCameraModeStack::GetCameraModeInstance(TSubclassOf<UModularCameraMode>& OutCameraModeView)
{
	check(OutCameraModeView);

	//CameraModeInstance에서 먼저 생성되어있는지 확인 후 반환한다.
	for (UModularCameraMode* CameraMode : CameraModeInstance)
	{
		//CameraMode는 UClass를 비교한다 -> CameraMode는 클래스 타입에 하나만 생기게됨
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == OutCameraModeView))
			return CameraMode;
	}

	//CameraModeClass에 알맞는 CameraMode의 인스턴스가 없다면 생성한다.
	UModularCameraMode* NewCameraMode = NewObject<UModularCameraMode>(GetOuter(), OutCameraModeView, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	CameraModeInstance.Add(NewCameraMode);

	return NewCameraMode;
}

void UCameraModeStack::PushCameraMode(TSubclassOf<UModularCameraMode>& CameraModeClass)
{
	if (!CameraModeClass)
		return;

	UModularCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 StackSize = CameraModeStack.Num();
	//가장 최근에 이미 CameraMode가 Stacking 되었으므로 그냥 리턴
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
		return;

	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.0f;

	/**
	 * BlendWeight    |    ExistingStackCOntribution    |    ExistingStackCOntribution (accumulated)
	 * 0.1f           |    (1.0f) * 0.1f = 0.1f         |    (1.0f - 0.1f) = 0.9f
	 * 0.3f           |    (0.9f) * 0.3f = 0.27f        |    (1.0f - 0.3f) * 0.9f = 0.63f
	 * 0.6f           |    (0.63f) * 0.6f = 0.378f      |    (1.0f - 0.6f) * 0.63f = 0.252f
	 * 1.0f           |    (0.252f) * 1.0f = 0.252f     |
	 *                |    0.1f + 0.27f + 0.378f + 0.252f = 1.0f!
	 */
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			ExistingStackIndex = StackIndex;
			ExistingStackContribution *= CameraMode->GetBlendWeight();
			break;
		}
		else
		{
			//원하는 CameraMode가 아니니까, InvBlendWeight = (1.0 - BlendWeight)를 곱해줘야, 값이 누적된다
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->GetBlendWeight());
		}
	}

	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		ExistingStackContribution = 0;
	}

	//BlendTime이 0보다 크다는 것은 Blend를 얼마 시간동안 진행함을 의미, 따라서 ExistingStackContribution을 적용
	const bool bShouldBlend = ((CameraMode->GetBlendTime() > 0.0f) && (StackSize > 0));

	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->SetBlendWeight(BlendWeight);

	CameraModeStack.Insert(CameraMode, 0);
	CameraModeStack.Last()->SetBlendWeight(1.0f);
}

void UCameraModeStack::EvaluateStack(float DeltaTime, FCameraModeView& OutCameraModeView)
{
	//Top -> Bottom [0 -> Num]까지 순차적으로 Stack에 있는 CameraMode 업데이트
	UpdateStack(DeltaTime);

	//Bottom -> Top 까지 CameraModeStack에 대해 Blending 진행
	BlendStack(OutCameraModeView);
}

void UCameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
		return;

	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;

	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UModularCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		CameraMode->UpdateCameraMode(DeltaTime);

		if (CameraMode->GetBlendWeight() >= 1.0f)
		{
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveIndex > 0)
	{
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UCameraModeStack::BlendStack(FCameraModeView& OutCameraModeView) const
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
		return;

	//CameraModeStack은 Botton->Top 순서로 Blend를 진행한다
	const UModularCameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	OutCameraModeView = CameraMode->GetCameraModeView();

	//이미 Index = [StackSize-1] 를 OutCameraModeView로 지정했으므로, StackSize - 2 부터 순회하면 된다.
	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		OutCameraModeView.Blend(CameraMode->GetCameraModeView(), CameraMode->GetBlendWeight());
	}
}
