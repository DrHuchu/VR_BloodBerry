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

	BB_SupporterCable = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SupporterCable"));
	BB_SupporterCable->SetupAttachment(BB_Main);
	BB_SupporterCable->SetCollisionProfileName(TEXT("NoCollision"));

	BB_ReceiverCableL = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReceiverCable_L"));
	BB_ReceiverCableL->SetupAttachment(BB_Main);
	BB_ReceiverCableL->SetCollisionProfileName(TEXT("NoCollision"));

	BB_ReceiverCableR = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReceiverCable_R"));
	BB_ReceiverCableR->SetupAttachment(BB_Main);
	BB_ReceiverCableR->SetCollisionProfileName(TEXT("NoCollision"));

	BB_Static_Folded = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticFolded"));
	BB_Static_Folded->SetupAttachment(BB_Main);
	BB_Static_Folded->SetCollisionProfileName(TEXT("PhysicsActor"));

	BB_Static_Released = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticReleased"));
	BB_Static_Released->SetupAttachment(BB_Main);
	BB_Static_Released->SetVisibility(false);
	BB_Static_Released->SetCollisionProfileName(TEXT("NoCollision"));

	BB_Static_Opened = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticOpened"));
	BB_Static_Opened->SetupAttachment(BB_Main);
	BB_Static_Opened->SetVisibility(false);
	BB_Static_Opened->SetCollisionProfileName(TEXT("NoCollision"));

	BB_Static_Closed = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticClosed"));
	BB_Static_Closed->SetupAttachment(BB_Main);
	BB_Static_Closed->SetVisibility(false);
	BB_Static_Closed->SetCollisionProfileName(TEXT("NoCollision"));

	BB_Static_Clicked = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticClicked"));
	BB_Static_Clicked->SetupAttachment(BB_Main);
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
	// 스태틱 메쉬 안 보이게, 충돌 X
	SetInvisible(BB_Static_Folded);
	// 스켈레탈 메쉬 보이게, 충돌 O
	SetVisible(BB_Main);
	// 애니메이션 몽타쥬 재생
	float Main_01_Duration = BB_Main->GetAnimInstance()->Montage_Play(Main_01, 1, EMontagePlayReturnType::Duration);

	FTimerHandle AnimationTimerHandle;
	GetWorldTimerManager().SetTimer(AnimationTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SetInvisible(BB_Main);
		SetVisible(BB_Static_Released);	
	}), Main_01_Duration, false);	
	
}
