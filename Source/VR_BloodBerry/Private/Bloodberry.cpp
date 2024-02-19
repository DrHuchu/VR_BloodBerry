// Fill out your copyright notice in the Description page of Project Settings.


#include "Bloodberry.h"
#include "Animation/AnimInstance.h"
#include "NiagaraComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ABloodberry::ABloodberry()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BB_Static = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticFolded"));
	SetRootComponent(BB_Static);
	BB_Static->SetCollisionProfileName(TEXT("PhysicsActor"));

	BB_Main = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BB_Main"));
	BB_Main->SetupAttachment(RootComponent);
	BB_Main->SetVisibility(false);
	BB_Main->SetCollisionProfileName(TEXT("NoCollision"));

	BB_SupporterCable = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SupporterCable"));
	BB_SupporterCable->SetupAttachment(RootComponent);
	BB_SupporterCable->SetCollisionProfileName(TEXT("NoCollision"));

	BB_ReceiverCableL = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReceiverCable_L"));
	BB_ReceiverCableL->SetupAttachment(RootComponent);
	BB_ReceiverCableL->SetCollisionProfileName(TEXT("NoCollision"));

	BB_ReceiverCableR = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReceiverCable_R"));
	BB_ReceiverCableR->SetupAttachment(RootComponent);
	BB_ReceiverCableR->SetCollisionProfileName(TEXT("NoCollision"));

	NI_Beam = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Beam"));
	NI_Beam->SetupAttachment(RootComponent);
	NI_Beam->SetRelativeLocation(FVector(0,0, 26.0f));
}

// Called when the game starts or when spawned
void ABloodberry::BeginPlay()
{
	Super::BeginPlay();

	if(CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &ABloodberry::BladeOn);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);

		// UpdateFloat.BindDynamic(this, &ABloodberry::BladeOn);
		// if(CurveFloat)
		// {
		// 	TimelineComp->AddInterpFloat(CurveFloat, UpdateFloat);
		// }
	}
	
	NI_Beam->SetVisibility(false);
	NI_Beam->SetNiagaraVariableFloat(FString(TEXT("BladeLength")), 0);

	//TimelineComp->Play();
	//NI_Beam->Deactivate();
}

void ABloodberry::BladeOn(float Value)
{
	BladeLength = Value;
	NI_Beam->SetNiagaraVariableFloat(FString(TEXT("BladeLength")), BladeLength);
}

// Called every frame
void ABloodberry::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Timeline.TickTimeline(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("%f!"), BladeLength);
}


void ABloodberry::SetInvisible(USceneComponent* Comp)
{
	if(USkeletalMeshComponent* SkeletalComp = Cast<USkeletalMeshComponent>(Comp))
	{
		SkeletalComp->SetVisibility(false);
		SkeletalComp->SetCollisionProfileName(TEXT("NoCollision"));
	}
	else if(UStaticMeshComponent* StaticComp = Cast<UStaticMeshComponent>(Comp))
	{
		StaticComp->SetVisibility(false);
		StaticComp->SetCollisionProfileName(TEXT("NoCollision"));
	}
	
}

void ABloodberry::SetVisible(USceneComponent* Comp)
{
	if(USkeletalMeshComponent* SkeletalComp = Cast<USkeletalMeshComponent>(Comp))
	{
		SkeletalComp->SetVisibility(true); 
		SkeletalComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	}
	else if(UStaticMeshComponent* StaticComp = Cast<UStaticMeshComponent>(Comp))
	{
		StaticComp->SetVisibility(true);
		StaticComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	}
}

void ABloodberry::SupporterRelease()
{
	if(bSupporterReleased || bReceiverReleased || bCoverOpened || bSwitchClicked || bIsPlaying)
	{
		return;
	}
	bIsPlaying = true;
	
	// 스태틱 메쉬 안 보이게, 충돌 X
	SetInvisible(BB_Static);
	// 스켈레탈 메쉬 보이게, 충돌 O
	SetVisible(BB_Main);
	// 메인 애니메이션 몽타쥬 재생
	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_01, 1, EMontagePlayReturnType::Duration);
	BB_SupporterCable->GetAnimInstance()->Montage_Play(Supporter_Release, 1);
	// 애니메이션 몽타쥬의 재생시간만큼 딜레이 부여
	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		// 스켈레탈 메쉬 안 보이게, 충돌 X
		SetInvisible(BB_Main);
		// 스태틱 메쉬 보이게, 충돌 O
		BB_Static->SetStaticMesh(Static_02);
		SetVisible(BB_Static);
		bSupporterReleased = true;
		bIsPlaying = false;
	}), Duration, false);	
}

void ABloodberry::SupporterUnrelease()
{
	if(!bSupporterReleased || bReceiverReleased || bCoverOpened || bSwitchClicked || bIsPlaying)
	{
		return;
	}
	bIsPlaying = true;
	SetInvisible(BB_Static);
	SetVisible(BB_Main);
	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_01, -1, EMontagePlayReturnType::Duration, 1);
	BB_SupporterCable->GetAnimInstance()->Montage_Play(Supporter_Release,-1, EMontagePlayReturnType::Duration, 1);
	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	// 애니메이션 몽타쥬 역재생이라 Duration 값에 -1을 곱해줘야 타이머 사용 가능
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		BB_Static->SetStaticMesh(Static_01);
		SetVisible(BB_Static);
		bSupporterReleased = false;
		bIsPlaying = false;
	}), Duration * -1, false);
}

void ABloodberry::ReceiverRelease()
{
	if(!bSupporterReleased || bReceiverReleased || bCoverOpened || bSwitchClicked || bIsPlaying)
	{
		return;
	}
	bIsPlaying = true;
	SetInvisible(BB_Static);
	SetVisible(BB_Main);
	
	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_02, 1, EMontagePlayReturnType::Duration, 0);
	BB_SupporterCable->GetAnimInstance()->Montage_Play(Supporter_Rotate, 1);
	BB_ReceiverCableL->GetAnimInstance()->Montage_Play(Receiver_L, 1);
	BB_ReceiverCableR->GetAnimInstance()->Montage_Play(Receiver_R, 1);
	
	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		BB_Static->SetStaticMesh(Static_03);
		SetVisible(BB_Static);
		bReceiverReleased = true;
		bIsPlaying = false;
	}), Duration, false);
}

void ABloodberry::ReceiverUnrelease()
{
	if(!bSupporterReleased || !bReceiverReleased || bCoverOpened || bSwitchClicked || bIsPlaying)
	{
		return;
	}
	bIsPlaying = true;
	SetInvisible(BB_Static);
	SetVisible(BB_Main);
	
	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_02, -1, EMontagePlayReturnType::Duration, 1);
	BB_SupporterCable->GetAnimInstance()->Montage_Play(Supporter_Rotate, -1, EMontagePlayReturnType::Duration, 1);
	BB_ReceiverCableL->GetAnimInstance()->Montage_Play(Receiver_L, -1, EMontagePlayReturnType::Duration, 1);
	BB_ReceiverCableR->GetAnimInstance()->Montage_Play(Receiver_R, -1, EMontagePlayReturnType::Duration, 1);
	
	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		BB_Static->SetStaticMesh(Static_02);
		SetVisible(BB_Static);
		bReceiverReleased = false;
		bIsPlaying = false;
	}), Duration * -1, false);
}

void ABloodberry::CoverOpen()
{
	if(!bSupporterReleased || !bReceiverReleased || bCoverOpened || bSwitchClicked || bIsPlaying)
	{
		return;
	}
	bIsPlaying = true;
	SetInvisible(BB_Static);
	SetVisible(BB_Main);

	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_03, 1, EMontagePlayReturnType::Duration, 0);

	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		BB_Static->SetStaticMesh(Static_04);
		SetVisible(BB_Static);
		bCoverOpened = true;
		bIsPlaying = false;
	}), Duration, false);
}

void ABloodberry::CoverClose()
{
	if(!bSupporterReleased || !bReceiverReleased || !bCoverOpened || bSwitchClicked || bIsPlaying)
	{
		return;
	}
	bIsPlaying = true;
	SetInvisible(BB_Static);
	SetVisible(BB_Main);

	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_03, -1, EMontagePlayReturnType::Duration, 1);

	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		BB_Static->SetStaticMesh(Static_03);
		SetVisible(BB_Static);
		bCoverOpened = false;
		bIsPlaying = false;
	}), Duration * -1, false);
}

void ABloodberry::SwitchClick()
{
	if(!bSupporterReleased || !bReceiverReleased || !bCoverOpened || bSwitchClicked || bIsPlaying)
	{
		return;
	}
	bIsPlaying = true;
	SetInvisible(BB_Static);
	SetVisible(BB_Main);

	NI_Beam->SetVisibility(true);
	//NI_Beam->Activate();
	// 빔 블레이드 나이아가라 Z 스케일값 늘리기
	Timeline.Play();

	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_04, 1, EMontagePlayReturnType::Duration, 0);

	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		BB_Static->SetStaticMesh(Static_05);
		SetVisible(BB_Static);
		bSwitchClicked = true;
		bIsPlaying = false;
	}), Duration, false);
	
}

void ABloodberry::SwitchUnclick()
{
	if(!bSupporterReleased || !bReceiverReleased || !bCoverOpened || !bSwitchClicked || bIsPlaying)
	{
		return;
	}
	bIsPlaying = true;
	SetInvisible(BB_Static);
	SetVisible(BB_Main);

	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_04, -1, EMontagePlayReturnType::Duration, 1);
	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		BB_Static->SetStaticMesh(Static_04);
		SetVisible(BB_Static);
		bSwitchClicked = false;
		bIsPlaying = false;
	}), Duration * -1, false);

	// 빔 블레이드 나이아가라 Z 스케일값 줄이기
	Timeline.Reverse();

	GetWorldTimerManager().ClearTimer(BeamTimerHandle);
	GetWorldTimerManager().SetTimer(BeamTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		NI_Beam->SetVisibility(false);
		//NI_Beam->Deactivate();
	}), 0.3f, false);
	
}

bool ABloodberry::IsSwinging(FVector CurrentLocation, FRotator CurrentRotation, float ThresholdAngle,
	float ThresholdSpeed)
{
	FVector Movement = CurrentLocation - PreviousLocation;
	FRotator RotationDelta = CurrentRotation - PreviousRotation;

	if(Movement.Size() >= ThresholdSpeed && FMath::Abs(RotationDelta.Yaw) <= ThresholdAngle)
	{
		return true;
	}
	
	return false;
}

void ABloodberry::UpdateVRController()
{
	FVector CurrentLocation = GetActorLocation();
	FRotator CurrentRotation = GetActorRotation();

	bool bIsSwinging = IsSwinging(CurrentLocation, CurrentRotation, 1.0f, 2.0f);

	if(bWasSwinging && !bIsSwinging)
	{
		SupporterRelease();
	}

	PreviousLocation = CurrentLocation;
	PreviousRotation = CurrentRotation;

	bWasSwinging = bIsSwinging;
}
