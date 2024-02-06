// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "VR_DefaultPawn.generated.h"

UCLASS()
class VR_BASIC_API AVR_DefaultPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVR_DefaultPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn")
	class USceneComponent* VR_Origin;
	
	//플레이어 이동속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	float moveSpeed = 0.5;

	//Input Mapping Context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_VRInput;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Hands;

	//Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Move;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Turn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Grab_R;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Grab_L;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Calibrate;

	//이동처리 함수
	UFUNCTION()
	void Move(const FInputActionValue& Values);
	
	//회전 함수 (스무스 턴)
	UFUNCTION()
	void SmoothTurn(const FInputActionValue& Values);

	//위치 재조정 함수 (캘리브레이션)
	UFUNCTION(BlueprintCallable, Category = "VR")
	void Calibrate();

	//회전 속도 (스무스 턴)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float SmoothTurnDegree = 0.3f;
	
	//사용할 손 모델
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionCotroller")
	class USkeletalMeshComponent* LeftHandMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionCotroller")
	class USkeletalMeshComponent* RightHandMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab")
	float GrabRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float SnapTurnDegree = -45.0f;
	
protected:
	//카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VRCamera")
	class UCameraComponent* VRCamera;

	//컨트롤러
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionController")
	class UMotionControllerComponent* LeftHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionController")
	class UMotionControllerComponent* RightHand;
};
