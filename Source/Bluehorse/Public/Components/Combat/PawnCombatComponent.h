// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"
#include "BluehorseTypes/BluehorseEnumTypes.h"
#include "PawnCombatComponent.generated.h"

// ïêäÌÇç∂âEÇ≈ï™ÇØÇÈenum
UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	PreEquip,
	LeftHand,
	RightHand
};

class ABluehorseWeaponBase;

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	void RegisterSpawnWeapon(FGameplayTag InWeaponTagToRegister, ABluehorseWeaponBase* InWeaponToRegister, EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	ABluehorseWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	UPROPERTY(BlueprintReadWrite, Category = "Bluehorse|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UPROPERTY(BlueprintReadWrite, Category = "Bluehorse|Combat")
	FGameplayTag EquippedLeftHandWeaponTag;

	UPROPERTY(BlueprintReadWrite, Category = "Bluehorse|Combat")
	FGameplayTag EquippedRightHandWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	ABluehorseWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	ABluehorseWeaponBase* GetCharacterEquippedRightHandWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	ABluehorseWeaponBase* GetCharacterEquippedLeftHandWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Combat")
	void ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType = EToggleDamageType::EquippedRightHandWeapon);

	// BluehorseWeaponBase.hÇ≈êÈåæÇµÇΩDelegate
	virtual void OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);

protected:
	// çUåÇÉqÉbÉgéûÇ…ÉqÉbÉgÇµÇΩActorÇäiî[ÇµÇƒÇ®Ç≠Ç‡ÇÃ
	TArray<AActor*> OverlappedActors;

private:
	TMap<FGameplayTag, ABluehorseWeaponBase*> CharacterCarriedWeaponMap;
};
