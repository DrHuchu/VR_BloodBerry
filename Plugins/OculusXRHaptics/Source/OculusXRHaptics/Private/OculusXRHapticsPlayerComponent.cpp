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

#include "OculusXRHapticsPlayerComponent.h"
#include "OculusXRHaptics.h"
#include "OculusXRHapticClip.h"
#include "Misc/AutomationTest.h"

UOculusXRHapticsPlayerComponent::UOculusXRHapticsPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOculusXRHapticsPlayerComponent::Play()
{
	PlayOnController(Controller);
}

void UOculusXRHapticsPlayerComponent::PlayOnController(const EOculusXRHapticController InController)
{

#if WITH_DEV_AUTOMATION_TESTS && !UE_BUILD_SHIPPING
	if (!FAutomationTestFramework::GetInstance().GetCurrentTest())
	{
		checkf(PlayerID != HAPTICS_SDK_INVALID_ID, TEXT("Trying to play with invalid player ID on player '%s'"), *GetName());
		checkf(ClipID != HAPTICS_SDK_INVALID_ID, TEXT("Trying to play with invalid clip ID on player '%s'"), *GetName());
	}
#endif
	if (HapticsModule == nullptr || PlayerID == HAPTICS_SDK_INVALID_ID || ClipID == HAPTICS_SDK_INVALID_ID)
	{
		return;
	}

	HapticsModule->HapticsSDKPlayerPlay(PlayerID, static_cast<HapticsSdkController>(InController));
}

void UOculusXRHapticsPlayerComponent::PlayWithInputs(const EOculusXRHapticController InController,
	const int32 InPriority, const float InAmplitude, const float InFrequencyShift, const bool bInIsLooping)
{
	SetController(InController);
	SetPriority(InPriority);
	SetAmplitude(InAmplitude);
	SetFrequencyShift(InFrequencyShift);
	SetLooping(bInIsLooping);
	Play();
}

void UOculusXRHapticsPlayerComponent::Stop()
{
	if (HapticsModule == nullptr || PlayerID == HAPTICS_SDK_INVALID_ID || ClipID == HAPTICS_SDK_INVALID_ID)
	{
		return;
	}

	HapticsModule->HapticsSDKPlayerStop(PlayerID);
}

UOculusXRHapticClip* UOculusXRHapticsPlayerComponent::GetHapticClip() const
{
	return HapticClip;
}

void UOculusXRHapticsPlayerComponent::SetHapticClip(UOculusXRHapticClip* InHapticClip)
{
	ReleaseClip();
	HapticClip = InHapticClip;
	LoadClipIntoPlayer();
}

EOculusXRHapticController UOculusXRHapticsPlayerComponent::GetController() const
{
	return Controller;
}

void UOculusXRHapticsPlayerComponent::SetController(const EOculusXRHapticController& InController)
{
	Controller = InController;
}

void UOculusXRHapticsPlayerComponent::SetPriority(const int32 InPriority)
{
	if (HapticsModule == nullptr || InPriority == Priority || PlayerID == HAPTICS_SDK_INVALID_ID)
	{
		return;
	}

	const HapticsSdkResult Result = HapticsModule->HapticsSDKPlayerSetPriority(PlayerID, static_cast<uint32_t>(InPriority));
	checkf(Result != HAPTICS_SDK_PLAYER_INVALID_PRIORITY,
		TEXT("Trying to set invalid value for priority (valid range is 0 to 1024) on player '%s'"), *GetName());
	if (HAPTICS_SDK_FAILED(Result))
	{
		return;
	}

	Priority = InPriority;
}

int32 UOculusXRHapticsPlayerComponent::GetPriority() const
{
	if (HapticsModule == nullptr || PlayerID == HAPTICS_SDK_INVALID_ID)
	{
		return Priority;
	}

	uint32_t OutPriority = 0;
	HapticsModule->HapticsSDKPlayerPriority(PlayerID, &OutPriority);
	if (Priority != static_cast<int32>(OutPriority))
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Priority out of sync"));
	}
	return OutPriority;
}

void UOculusXRHapticsPlayerComponent::SetAmplitude(const float InAmplitude)
{
	if (HapticsModule == nullptr || InAmplitude == Amplitude || PlayerID == HAPTICS_SDK_INVALID_ID)
	{
		return;
	}

	const HapticsSdkResult Result = HapticsModule->HapticsSDKPlayerSetAmplitude(PlayerID, InAmplitude);
	checkf(Result != HAPTICS_SDK_PLAYER_INVALID_AMPLITUDE,
		TEXT("Trying to set invalid value for amplitude (must be 0 or higher) on player '%s'"), *GetName());
	if (HAPTICS_SDK_FAILED(Result))
	{
		return;
	}

	Amplitude = InAmplitude;
}

float UOculusXRHapticsPlayerComponent::GetAmplitude() const
{
	if (HapticsModule == nullptr || PlayerID == HAPTICS_SDK_INVALID_ID)
	{
		return Amplitude;
	}

	float OutAmplitude = 0;
	HapticsModule->HapticsSDKPlayerAmplitude(PlayerID, &OutAmplitude);
	if (Amplitude != OutAmplitude)
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Amplitude out of sync"));
	}
	return OutAmplitude;
}

void UOculusXRHapticsPlayerComponent::SetFrequencyShift(const float InFrequencyShift)
{
	if (HapticsModule == nullptr || InFrequencyShift == FrequencyShift || PlayerID == HAPTICS_SDK_INVALID_ID)
	{
		return;
	}

	const HapticsSdkResult Result = HapticsModule->HapticsSDKPlayerSetFrequencyShift(PlayerID, InFrequencyShift);
	checkf(Result != HAPTICS_SDK_PLAYER_INVALID_FREQUENCY_SHIFT,
		TEXT("Trying to set invalid value for frequency shift (valid range is -1 to 1) on player '%s'"), *GetName());
	if (HAPTICS_SDK_FAILED(Result))
	{
		return;
	}

	FrequencyShift = InFrequencyShift;
}

float UOculusXRHapticsPlayerComponent::GetFrequencyShift() const
{
	if (HapticsModule == nullptr || PlayerID == HAPTICS_SDK_INVALID_ID)
	{
		return FrequencyShift;
	}

	float OutFrequencyShift = 0;
	HapticsModule->HapticsSDKPlayerFrequencyShift(PlayerID, &OutFrequencyShift);
	if (FrequencyShift != OutFrequencyShift)
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Frequency shift out of sync"));
	}
	return OutFrequencyShift;
}

void UOculusXRHapticsPlayerComponent::SetLooping(const bool bInIsLooping)
{
	if (HapticsModule == nullptr || bInIsLooping == bIsLooping || PlayerID == HAPTICS_SDK_INVALID_ID)
	{
		return;
	}

	HapticsModule->HapticsSDKPlayerSetLoopingEnabled(PlayerID, bInIsLooping);
	bIsLooping = bInIsLooping;
}

bool UOculusXRHapticsPlayerComponent::GetLooping() const
{
	if (HapticsModule == nullptr || PlayerID == HAPTICS_SDK_INVALID_ID)
	{
		return bIsLooping;
	}

	bool bOutIsLooping = false;
	HapticsModule->HapticsSDKPlayerLoopingEnabled(PlayerID, &bOutIsLooping);
	if (bIsLooping != bOutIsLooping)
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Looping out of sync"));
	}
	return bOutIsLooping;
}

float UOculusXRHapticsPlayerComponent::GetClipDuration() const
{
	if (HapticsModule == nullptr || ClipID == HAPTICS_SDK_INVALID_ID)
	{
		return 0;
	}

	float OutClipDuration = 0;
	HapticsModule->HapticsSDKClipDuration(ClipID, &OutClipDuration);
	return OutClipDuration;
}

void UOculusXRHapticsPlayerComponent::SetInitialValues(UOculusXRHapticClip* InHapticClip,
	const EOculusXRHapticController InController, const int32 InPriority, const float InAmplitude,
	const float InFrequencyShift, const bool bInIsLooping)
{
	HapticClip = InHapticClip;
	Controller = InController;
	Priority = InPriority;
	Amplitude = InAmplitude;
	FrequencyShift = InFrequencyShift;
	bIsLooping = bInIsLooping;
}

void UOculusXRHapticsPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	HapticsModule = FOculusXRHapticsModule::GetIfLibraryLoaded();
	if (HapticsModule == nullptr)
	{
		return;
	}

	HapticsModule->HapticsSDKCreatePlayer(&PlayerID);
	if (PlayerID == HAPTICS_SDK_INVALID_ID)
	{
		return;
	}

	HapticsModule->HapticsSDKPlayerSetPriority(PlayerID, static_cast<uint32_t>(Priority));
	HapticsModule->HapticsSDKPlayerSetAmplitude(PlayerID, Amplitude);
	HapticsModule->HapticsSDKPlayerSetFrequencyShift(PlayerID, FrequencyShift);
	HapticsModule->HapticsSDKPlayerSetLoopingEnabled(PlayerID, bIsLooping);

	LoadClipIntoPlayer();
}

void UOculusXRHapticsPlayerComponent::EndPlay(EEndPlayReason::Type reason)
{
	Super::EndPlay(reason);

	if (HapticsModule && PlayerID != HAPTICS_SDK_INVALID_ID)
	{
		HapticsModule->HapticsSDKReleasePlayer(PlayerID);
		PlayerID = HAPTICS_SDK_INVALID_ID;
	}

	ReleaseClip();
}

void UOculusXRHapticsPlayerComponent::LoadClipIntoPlayer()
{
	if (HapticsModule == nullptr || PlayerID == HAPTICS_SDK_INVALID_ID)
	{
		return;
	}

	if (HapticClip == nullptr || HapticClip->ClipData.Num() == 0)
	{
		UE_LOG(LogHapticsSDK, Warning, TEXT("Empty haptic clip set for player '%s', will not load"), *GetName());
		return;
	}

	HapticsModule->HapticsSDKLoadClip(reinterpret_cast<const char*>(HapticClip->ClipData.GetData()),
		HapticClip->ClipData.Num(), &ClipID);
	if (ClipID == HAPTICS_SDK_INVALID_ID)
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Failed to load haptic clip file"));
		return;
	}

	HapticsModule->HapticsSDKPlayerSetClip(PlayerID, ClipID);
}

void UOculusXRHapticsPlayerComponent::ReleaseClip()
{
	if (HapticsModule == nullptr || ClipID == HAPTICS_SDK_INVALID_ID)
	{
		return;
	}

	HapticsModule->HapticsSDKReleaseClip(ClipID);
	ClipID = HAPTICS_SDK_INVALID_ID;
}
