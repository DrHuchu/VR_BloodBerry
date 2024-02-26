/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 *
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "OculusXRHapticsFunctionLibrary.h"

#include "OculusXRHaptics.h"
#include "OculusXRHapticsPlayerComponent.h"
#include "GameFramework/Actor.h"

UOculusXRHapticsPlayerComponent* UOculusXRHapticsFunctionLibrary::SpawnHapticsPlayerComponent(
	AActor* ActorToAttachTo,
	UOculusXRHapticClip* HapticClip,
	const EOculusXRHapticController Controller,
	const int32 Priority,
	const float Amplitude,
	const float FrequencyShift,
	const bool bIsLooping)
{
	checkf(ActorToAttachTo != nullptr, TEXT("SpawnHapticsPlayerComponent function called with null actor"));
	if (ActorToAttachTo == nullptr)
	{
		return nullptr;
	}

	UOculusXRHapticsPlayerComponent* const NewPlayerComponent = Cast<UOculusXRHapticsPlayerComponent>(
		ActorToAttachTo->AddComponentByClass(UOculusXRHapticsPlayerComponent::StaticClass(), false,
			FTransform::Identity, true));
	if (NewPlayerComponent == nullptr)
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Player component creation failed"));
		return nullptr;
	}

	NewPlayerComponent->SetInitialValues(HapticClip, Controller,
		Priority, Amplitude, FrequencyShift, bIsLooping);
	ActorToAttachTo->FinishAddComponent(NewPlayerComponent, false, FTransform::Identity);
	return NewPlayerComponent;
}
