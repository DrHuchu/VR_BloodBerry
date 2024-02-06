// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "VR_Mannequins.generated.h"

/**
 * 
 */

UCLASS()
class VR_BASIC_API UVR_Mannequins : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ABP")
	bool bMirror;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ABP")
	float PoseAlphaGrasp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ABP")
	float PoseAlphaIndexCurl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ABP")
	float PoseAlphaPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ABP")
	float PoseAlphaThumbup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ABP")
	float CurrentPoseAlphaThumbUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ABP")
	float CurrentPoseAlphaPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ABP")
	int32 GrabbedObjectInt = 0;
};

