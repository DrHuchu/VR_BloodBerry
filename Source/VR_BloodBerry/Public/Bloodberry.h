// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "Components/TimelineComponent.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Haptics/HapticFeedbackEffect_SoundWave.h"
#include "Bloodberry.generated.h"

UCLASS()
class VR_BLOODBERRY_API ABloodberry : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABloodberry();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BB")
	USkeletalMeshComponent* BB_Main;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BB")
	USkeletalMeshComponent* BB_SupporterCable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BB")
	USkeletalMeshComponent* BB_ReceiverCableL;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BB")
	USkeletalMeshComponent* BB_ReceiverCableR;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BB")
	UStaticMeshComponent* BB_Static;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
	UStaticMesh* Static_01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
	UStaticMesh* Static_02;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
	UStaticMesh* Static_03;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
	UStaticMesh* Static_04;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
	UStaticMesh* Static_05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* Main_01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* Main_02;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* Main_03;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* Main_04;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* Receiver_R;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* Receiver_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* Supporter_Release;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* Supporter_Rotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam")
	UNiagaraComponent* NI_Beam;

	UPROPERTY()
	FTimeline Timeline;

	UPROPERTY(VisibleAnywhere, Category = "Beam")
	UTimelineComponent* TimelineComp;
	
	UPROPERTY(EditAnywhere, Category = "Beam")
	UCurveFloat* CurveFloat;

	FOnTimelineFloat UpdateFloat;

	FTimerHandle BeamTimerHandle;

	UFUNCTION()
	void BladeOn(float Value);

	float BladeLength;

	UFUNCTION(BlueprintCallable)
	void SetInvisible(USceneComponent* Comp);

	UFUNCTION(BlueprintCallable)
	void SetVisible(USceneComponent* Comp);

	UPROPERTY(VisibleAnywhere, Category = "Anim")
	bool bIsPlaying;

	// 서포터 릴리즈 애니메이션 관련
	
	UFUNCTION(BlueprintCallable)
	void SupporterRelease();

	FTimerHandle AnimationTimerHandle;
	
	UFUNCTION(BLueprintCallable)
	void SupporterUnrelease();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Anim")
	bool bSupporterReleased;

	// 리시버 릴리즈 애니메이션 관련
	
	UFUNCTION(BlueprintCallable)
	void ReceiverRelease();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Anim")
	bool bReceiverReleased;

	UFUNCTION(BlueprintCallable)
	void ReceiverUnrelease();

	// 커버 오픈 애니메이션 관련

	UFUNCTION(BlueprintCallable)
	void CoverOpen();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Anim")
	bool bCoverOpened;

	UFUNCTION(BlueprintCallable)
	void CoverClose();

	// 스위치 클릭 애니메이션 관련

	UFUNCTION(BlueprintCallable)
	bool SwitchClick();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Anim")
	bool bSwitchClicked;

	UFUNCTION(BlueprintCallable)
	bool SwitchUnclick();

	// 속도 / 각도 추적
	FVector PreviousLocation;
	FRotator PreviousRotation;

	UPROPERTY(BlueprintReadWrite, Category = "Vector")
	FVector SliceDirection;

	UFUNCTION()
	bool IsSwinging(FVector CurrentLocation, FRotator CurrentRotation, float ThresholdAngle, float ThresholdSpeed);

	UFUNCTION(BlueprintCallable)
	void UpdateVRController();

	bool bWasSwinging = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsGrabbed;

	UFUNCTION(BlueprintCallable)
	float UpdateVRSpeed(FVector& PrevLoc, FVector& CurLoc);

	// 사운드 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	UAudioComponent* AudioComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* S_TurnOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* S_TurnOff;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* S_Running;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* S_Supporter_Release;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* S_Receiver_Release;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* S_Receiver_Unrelease;
	
	// 햅틱 관련
	UPROPERTY(EditAnywhere, Category = "Haptic")
	UHapticFeedbackEffect_Curve* H_TurnOn;

	UPROPERTY(EditAnywhere, Category = "Haptic")
	UHapticFeedbackEffect_Curve* H_TurnOff;

	UPROPERTY(EditAnywhere, Category = "Haptic")
	UHapticFeedbackEffect_Curve* H_Running;
};
