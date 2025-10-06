// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "HeroCombatComponent.generated.h"

class ABluehorseHeroWeapon;

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	ABluehorseHeroWeapon* GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const;

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	ABluehorseHeroWeapon* GetHeroEquippedRightHandWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	ABluehorseHeroWeapon* GetHeroEquippedLeftHandWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	float GetHeroEquippedRightHandWeaponDamageAtLevel(float InLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	float GetHeroEquippedLeftHandWeaponDamageAtLevel(float InLevel) const;

	// BluehorseWeaponBase.hÇ≈êÈåæÇµÇΩDelegate
	virtual void OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult) override;
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;
};
