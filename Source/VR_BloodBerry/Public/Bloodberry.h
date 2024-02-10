// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	UStaticMeshComponent* BB_Static_Folded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BB")
	UStaticMeshComponent* BB_Static_Released;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BB")
	UStaticMeshComponent* BB_Static_Opened;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BB")
	UStaticMeshComponent* BB_Static_Closed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BB")
	UStaticMeshComponent* BB_Static_Clicked;

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

	UFUNCTION(BlueprintCallable)
	void SetInvisible(USceneComponent* Comp);

	UFUNCTION(BlueprintCallable)
	void SetVisible(USceneComponent* Comp);
	
	UFUNCTION(BlueprintCallable)
	void SupporterRelease();
	
	//void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	//void OnMontageCompleted();
};
