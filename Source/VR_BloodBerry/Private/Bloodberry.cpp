// Fill out your copyright notice in the Description page of Project Settings.


#include "Bloodberry.h"
#include "PlayMontageCallbackProxy.h"
#include "Animation/AnimInstance.h"
#include "Dataflow/DataflowEngineUtil.h"

// Sets default values
ABloodberry::ABloodberry()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BB_Main = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BB_Main"));
	SetRootComponent(BB_Main);
	BB_Main->SetVisibility(false);
	BB_Main->SetCollisionProfileName(TEXT("NoCollision"));
	
	BB_Static_Folded = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticFolded"));
	BB_Static_Folded->SetupAttachment(BB_Main);
	BB_Static_Folded->SetCollisionProfileName(TEXT("PhysicsActor"));

	BB_SupporterCable = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SupporterCable"));
	BB_SupporterCable->SetupAttachment(RootComponent);
	BB_SupporterCable->SetCollisionProfileName(TEXT("NoCollision"));

	BB_ReceiverCableL = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReceiverCable_L"));
	BB_ReceiverCableL->SetupAttachment(RootComponent);
	BB_ReceiverCableL->SetCollisionProfileName(TEXT("NoCollision"));

	BB_ReceiverCableR = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReceiverCable_R"));
	BB_ReceiverCableR->SetupAttachment(RootComponent);
	BB_ReceiverCableR->SetCollisionProfileName(TEXT("NoCollision"));
	
	BB_Static_Released = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticReleased"));
	BB_Static_Released->SetupAttachment(RootComponent);
	BB_Static_Released->SetVisibility(false);
	BB_Static_Released->SetCollisionProfileName(TEXT("NoCollision"));

	BB_Static_Opened = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticOpened"));
	BB_Static_Opened->SetupAttachment(RootComponent);
	BB_Static_Opened->SetVisibility(false);
	BB_Static_Opened->SetCollisionProfileName(TEXT("NoCollision"));

	BB_Static_Closed = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticClosed"));
	BB_Static_Closed->SetupAttachment(RootComponent);
	BB_Static_Closed->SetVisibility(false);
	BB_Static_Closed->SetCollisionProfileName(TEXT("NoCollision"));

	BB_Static_Clicked = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticClicked"));
	BB_Static_Clicked->SetupAttachment(RootComponent);
	BB_Static_Clicked->SetVisibility(false);
	BB_Static_Clicked->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void ABloodberry::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABloodberry::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	SetInvisible(BB_Static_Folded);
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
		SetVisible(BB_Static_Released);
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
	SetInvisible(BB_Static_Released);
	SetVisible(BB_Main);
	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_01, -1, EMontagePlayReturnType::Duration, 1);
	BB_SupporterCable->GetAnimInstance()->Montage_Play(Supporter_Release,-1, EMontagePlayReturnType::Duration, 1);
	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	// 애니메이션 몽타쥬 역재생이라 Duration 값에 -1을 곱해줘야 타이머 사용 가능
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		UE_LOG(LogTemp, Warning, TEXT("Fold"));
		SetInvisible(BB_Main);
		SetVisible(BB_Static_Folded);
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
	SetInvisible(BB_Static_Released);
	SetVisible(BB_Main);
	
	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_02, 1, EMontagePlayReturnType::Duration, 0);
	BB_SupporterCable->GetAnimInstance()->Montage_Play(Supporter_Rotate, 1);
	BB_ReceiverCableL->GetAnimInstance()->Montage_Play(Receiver_L, 1);
	BB_ReceiverCableR->GetAnimInstance()->Montage_Play(Receiver_R, 1);
	
	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		SetVisible(BB_Static_Closed);
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
	SetInvisible(BB_Static_Closed);
	SetVisible(BB_Main);
	
	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_02, -1, EMontagePlayReturnType::Duration, 1);
	BB_SupporterCable->GetAnimInstance()->Montage_Play(Supporter_Rotate, -1, EMontagePlayReturnType::Duration, 1);
	BB_ReceiverCableL->GetAnimInstance()->Montage_Play(Receiver_L, -1, EMontagePlayReturnType::Duration, 1);
	BB_ReceiverCableR->GetAnimInstance()->Montage_Play(Receiver_R, -1, EMontagePlayReturnType::Duration, 1);
	
	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		SetVisible(BB_Static_Released);
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
	SetInvisible(BB_Static_Closed);
	SetVisible(BB_Main);

	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_03, 1, EMontagePlayReturnType::Duration, 0);

	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		SetVisible(BB_Static_Opened);
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
	SetInvisible(BB_Static_Opened);
	SetVisible(BB_Main);

	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_03, -1, EMontagePlayReturnType::Duration, 1);

	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		SetVisible(BB_Static_Closed);
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
	SetInvisible(BB_Static_Opened);
	SetVisible(BB_Main);

	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_04, 1, EMontagePlayReturnType::Duration, 0);

	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		SetVisible(BB_Static_Clicked);
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
	SetInvisible(BB_Static_Clicked);
	SetVisible(BB_Main);

	float Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_04, -1, EMontagePlayReturnType::Duration, 1);
	GetWorldTimerManager().ClearTimer(AnimationTimerHandle);
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		SetVisible(BB_Static_Opened);
		bSwitchClicked = false;
		bIsPlaying = false;
	}), Duration * -1, false);
}

