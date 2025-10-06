// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "HeroUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, TSoftObjectPtr<UTexture2D>, SoftWeaponIcon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSlotChangedDelegate, TSoftObjectPtr<UTexture2D>, SoftItemIcon, int32, ItemCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemSlotRemoveDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemConsumeDelegate, TSoftObjectPtr<UTexture2D>, SoftItemIcon, int32, ItemCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemInitDelegate, TSoftObjectPtr<UTexture2D>, SoftItemIcon, int32, ItemCount);


/**
 * 
 */
UCLASS()
class BLUEHORSE_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangeDelegate OnCurrentSkillPointChanged; // êeÉNÉâÉXÇ≈êÈåæçœÇ›ÇÃDelegate

	UPROPERTY(BlueprintAssignable)
	FOnPercentChangeDelegate OnCurrentStaminaChanged;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnItemSlotChangedDelegate OnItemSlotChanged;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnItemSlotRemoveDelegate OnItemSlotRemove;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnItemConsumeDelegate OnItemConsume;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnItemInitDelegate OnItemSlotInit;
}; 
