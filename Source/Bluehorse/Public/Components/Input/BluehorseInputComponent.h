// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "BluehorseInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// Tag��p����NativeAction���o�C���h����e���v���[�g�֐�
	template<class UserObject, typename CallbackFunc>
	void BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func);
	
	// Ability�n�̓��͂��܂Ƃ߂ăo�C���h����e���v���[�g�֐�
	template<class UserObject, typename CallbackFunc>
	void BindAbilityInputAction(const UDataAsset_InputConfig* InInputConfig, UserObject* ContextObject, CallbackFunc InputPressedFunc, CallbackFunc InputReleasedFunc);
};

template<class UserObject, typename CallbackFunc>
inline void UBluehorseInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	checkf(InInputConfig, TEXT("Input config data asset is null, can not proceed with binding"));

	// ���͗p��DataAsset����w�肵��Tag�ɕR�Â���UInputAction���擾
	if (UInputAction* FoundAction = InInputConfig->FindNativeInputActionByTag(InInputTag))
	{
		// UInputAction�ɑ΂��Ďw���CallbackFunc���o�C���h
		BindAction(FoundAction, TriggerEvent, ContextObject, Func);
	}
}

template<class UserObject, typename CallbackFunc>
inline void UBluehorseInputComponent::BindAbilityInputAction(const UDataAsset_InputConfig* InInputConfig, UserObject* ContextObject, CallbackFunc InputPressedFunc, CallbackFunc InputReleasedFunc)
{
	checkf(InInputConfig, TEXT("Input config data asset is null, can not proceed with binding"));

	// ���͗p��DataAsset�������m�F
	for (const FBluehorseInputActionConfig& AbilityInputActionConfig : InInputConfig->AbilityInputActions)
	{
		if (!AbilityInputActionConfig.IsValid()) continue;

		// �����E���������̃o�C���h
		BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started, ContextObject, InputPressedFunc, AbilityInputActionConfig.InputTag);
		BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Completed, ContextObject, InputReleasedFunc, AbilityInputActionConfig.InputTag);
	}
}
