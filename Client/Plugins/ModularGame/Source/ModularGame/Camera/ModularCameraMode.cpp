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
		//Weight�� 1.0�̸� Other�� ����� �ȴ�.
		*this = Other;
		return;
	}

	//Location + OtherWeight * (Other.Location - Location)
	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	//Location�� ���� ��� Lerp (ControlRotation�� FieldOfView�� ����)
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
	//Actor�� Ȱ���Ͽ�, Pivot[Location|Rotation]�� ����Ͽ�, View�� ������Ʈ
	UpdateView(DeltaTime);

	//BlendWeight�� DeltaTime�� Ȱ���Ͽ�, BlendAlpha ��� ��, BlendFunction�� �°� �� �����Ͽ� ���� ���
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
{//BlendAlpha�� DeltaTime�� ���� ���
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
	//UPTCameraModeStack::GetCameraModeInstacne()Ȯ��
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

	//BaseEyeHeight�� ����Ͽ�, ViewLocation�� ��ȯ
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

	//GetViewRotation() Ȯ��, ���� Pawn�� ControlRotation�� ��ȯ
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

	//CameraModeInstance���� ���� �����Ǿ��ִ��� Ȯ�� �� ��ȯ�Ѵ�.
	for (UModularCameraMode* CameraMode : CameraModeInstance)
	{
		//CameraMode�� UClass�� ���Ѵ� -> CameraMode�� Ŭ���� Ÿ�Կ� �ϳ��� ����Ե�
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == OutCameraModeView))
			return CameraMode;
	}

	//CameraModeClass�� �˸´� CameraMode�� �ν��Ͻ��� ���ٸ� �����Ѵ�.
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
	//���� �ֱٿ� �̹� CameraMode�� Stacking �Ǿ����Ƿ� �׳� ����
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
			//���ϴ� CameraMode�� �ƴϴϱ�, InvBlendWeight = (1.0 - BlendWeight)�� �������, ���� �����ȴ�
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

	//BlendTime�� 0���� ũ�ٴ� ���� Blend�� �� �ð����� �������� �ǹ�, ���� ExistingStackContribution�� ����
	const bool bShouldBlend = ((CameraMode->GetBlendTime() > 0.0f) && (StackSize > 0));

	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->SetBlendWeight(BlendWeight);

	CameraModeStack.Insert(CameraMode, 0);
	CameraModeStack.Last()->SetBlendWeight(1.0f);
}

void UCameraModeStack::EvaluateStack(float DeltaTime, FCameraModeView& OutCameraModeView)
{
	//Top -> Bottom [0 -> Num]���� ���������� Stack�� �ִ� CameraMode ������Ʈ
	UpdateStack(DeltaTime);

	//Bottom -> Top ���� CameraModeStack�� ���� Blending ����
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

	//CameraModeStack�� Botton->Top ������ Blend�� �����Ѵ�
	const UModularCameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	OutCameraModeView = CameraMode->GetCameraModeView();

	//�̹� Index = [StackSize-1] �� OutCameraModeView�� ���������Ƿ�, StackSize - 2 ���� ��ȸ�ϸ� �ȴ�.
	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		OutCameraModeView.Blend(CameraMode->GetCameraModeView(), CameraMode->GetBlendWeight());
	}
}
