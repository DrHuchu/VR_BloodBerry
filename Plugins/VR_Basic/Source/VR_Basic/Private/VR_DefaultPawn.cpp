// Fill out your copyright notice in the Description page of Project Settings.


#include "VR_DefaultPawn.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "RepMovementNetSerializer.h"
#include "SWarningOrErrorBox.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/LocalPlayer.h"

// Sets default values
AVR_DefaultPawn::AVR_DefaultPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VR_Origin = CreateDefaultSubobject<USceneComponent>(TEXT("VR_Origin"));
	VR_Origin->SetupAttachment(RootComponent);
	//SetRootComponent(VR_Origin);
	
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(VR_Origin);
	VRCamera->bUsePawnControlRotation = true;
	VRCamera->bUsePawnControlRotation = false;

	LeftHand = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHand"));
	LeftHand->SetupAttachment(VR_Origin);
	LeftHand->SetTrackingMotionSource(FName("Left"));
	
	RightHand = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHand"));
	RightHand->SetupAttachment(VR_Origin);
	RightHand->SetTrackingMotionSource(FName("Right"));

	//스켈레탈 메쉬 컴포넌트
	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh"));
	LeftHandMesh->SetupAttachment(LeftHand);
	LeftHandMesh->SetRelativeLocation(FVector(-2.9f,-3.5f, 4.5f));
	LeftHandMesh->SetRelativeRotation(FRotator(-25.0f, -180.0f, 90.0f));
	
	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(RightHand);
	RightHandMesh->SetRelativeLocation(FVector(-2.9f, 3.5f, 4.5f));
	RightHandMesh->SetRelativeRotation(FRotator(25.0f, 0.0f, 90.0f));

	//스켈레탈 메쉬 할당
	// ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh (TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_left.SKM_MannyXR_left'"));
	// if(TempMesh.Succeeded())
	// {
	// 	LeftHandMesh->SetSkeletalMesh(TempMesh.Object);
	// 	LeftHandMesh->SetRelativeLocation(FVector(-2.9f,-3.5f, 4.5f));
	// 	LeftHandMesh->SetRelativeRotation(FRotator(-25.0f, -180.0f, 90.0f));
	// }
	// ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh2 (TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_right.SKM_MannyXR_right'"));
	// if(TempMesh2.Succeeded())
	// {
	// 	RightHandMesh->SetSkeletalMesh(TempMesh2.Object);
	// 	RightHandMesh->SetRelativeLocation(FVector(-2.9f, 3.5f, 4.5f));
	// 	RightHandMesh->SetRelativeRotation(FRotator(25.0f, 0.0f, 90.0f));
	// }

	bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void AVR_DefaultPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//Enhanced Input 사용처리
	auto PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

	if(PC)
	{
		// Local Player
		auto localPlayer = PC->GetLocalPlayer();
		auto subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer);

		if(subSystem)
		{
			subSystem->AddMappingContext(IMC_VRInput, 0);
			subSystem->AddMappingContext(IMC_Hands, 0);
		}
	}

	if(UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		// Standing mode의 트래킹 오리진 -> Floor (카메라 높이 바닥)
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
		// Sitting mode의 트래킹 오리진 -> Eye (카메라 높이도 올려줘야 함)
		//UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
	}
}

// Called every frame
void AVR_DefaultPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

// Called to bind functionality to input
void AVR_DefaultPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	auto InputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if(InputSystem)
	{
		//Binding
		InputSystem->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AVR_DefaultPawn::Move);
		InputSystem->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AVR_DefaultPawn::SmoothTurn);
	}

}

void AVR_DefaultPawn::Move(const FInputActionValue& Values)
{
	//컨트롤러 입력 값에 따라 전후좌우 이동
	FVector2D Axis = Values.Get<FVector2D>();
	AddMovementInput(VRCamera->GetForwardVector(), Axis.Y * moveSpeed);
	AddMovementInput(VRCamera->GetRightVector(), Axis.X * moveSpeed);
}

void AVR_DefaultPawn::SmoothTurn(const FInputActionValue& Values)
{
	float TurnValue = Values.Get<float>();
	AddControllerYawInput(TurnValue * SmoothTurnDegree);
}

void AVR_DefaultPawn::Calibrate()
{
	//캡슐 컴포넌트의 위치를 카메라의 월드 위치로 이동
	SetActorLocation(FVector(VRCamera->GetComponentLocation().X,VRCamera->GetComponentLocation().Y,GetRootComponent()->GetComponentLocation().Z));
	//캡슐 컴포넌트의 회전값을 카메라의 회전 값으로
	SetActorRotation(FRotator(0,VRCamera->GetComponentRotation().Yaw,0));
	//위치 재정의
	//UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::OrientationAndPosition);
}

bool AVR_DefaultPawn::IsSwingingL(FVector CurrentLocation, FRotator CurrentRotation, float ThresholdAngle,
	float ThresholdSpeed)
{
	FVector Movement = CurrentLocation - PreviousControllerLocationL;
	FRotator RotationDelta = CurrentRotation - PreviousControllerRotationL;

	if(Movement.Size() >= ThresholdSpeed && FMath::Abs(RotationDelta.Yaw) <= ThresholdAngle)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateL"));
		return true;
	}
	
	return false;
}

bool AVR_DefaultPawn::IsSwingingR(FVector CurrentLocation, FRotator CurrentRotation, float ThresholdAngle,
	float ThresholdSpeed)
{
	FVector Movement = CurrentLocation - PreviousControllerLocationR;
	FRotator RotationDelta = CurrentRotation - PreviousControllerRotationR;

	UE_LOG(LogTemp, Warning, TEXT("Swing Check R"));
	UE_LOG(LogTemp, Warning, TEXT("Movement : %f"), Movement.Size());
	UE_LOG(LogTemp, Warning, TEXT("Rotation : %f"), FMath::Abs(RotationDelta.Yaw));

	if(Movement.Size() >= ThresholdSpeed && FMath :: Abs(RotationDelta.Yaw) <= ThresholdAngle)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateR"));
		return true;
	}
	
	return false;
}

void AVR_DefaultPawn::UpdateVRControllerL()
{
	FVector CurrentControllerLocation = LeftHand->GetComponentLocation();
	FRotator CurrentControllerRotation = LeftHand->GetComponentRotation();

	bool bIsSwinging = IsSwingingL(CurrentControllerLocation, CurrentControllerRotation, 1.0f, 2.0f);
	
	if(bWasSwingingL && !bIsSwinging)
	{
		ControllerStopped();
	}

	PreviousControllerLocationL = CurrentControllerLocation;
	PreviousControllerRotationL = CurrentControllerRotation;

	bWasSwingingL = bIsSwinging;
}

void AVR_DefaultPawn::UpdateVRControllerR()
{
	FVector CurrentControllerLocation = RightHand->GetComponentLocation();
	FRotator CurrentControllerRotation = RightHand->GetComponentRotation();
	
	bool bIsSwinging = IsSwingingR(CurrentControllerLocation, CurrentControllerRotation, 1.0f, 2.0f);

	if(bWasSwingingR && !bIsSwinging)
	{
		ControllerStopped();
	}

	PreviousControllerLocationR = CurrentControllerLocation;
	PreviousControllerRotationR = CurrentControllerRotation;

	bWasSwingingR = bIsSwinging;
}


