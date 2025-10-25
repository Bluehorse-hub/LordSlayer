// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BluehorseHeroGameplayAbility.h"
#include "GA_HeroAttackMaster.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UGA_HeroAttackMaster : public UBluehorseHeroGameplayAbility
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable)
	void RemoveTagsOnEndAbility();

	UFUNCTION(BlueprintCallable)
	FGameplayEffectSpecHandle GetHeroEquippedRightHandWeaponDamageSpecHandle();

	UFUNCTION(BlueprintCallable)
	FGameplayEffectSpecHandle GetHeroEquippedLeftHandWeaponDamageSpecHandle();

	UFUNCTION(BlueprintCallable)
	void HandleApplyDamage(const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable)
	float GetMotionValueFromDataTable();

	UFUNCTION(BlueprintCallable)
	void ComputeAttackDirection();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UDataTable* MotionValueDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AttackName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CurrentAttackTypeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGameplayTag> TagsToRemoveOnEnd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsRightWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName NormalAttackWarpTargetName = "AttackingDirection";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName FatalAttackWarpTargetName = "FatalAttackTarget";
};

