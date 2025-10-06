// Shungen All Rights Reserved.

#pragma once
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "BluehorseStructTypes.generated.h"

class UBluehorseHeroLinkedAnimLayer;
class UBluehorseGameplayAbility;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FBluehorseHeroAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UBluehorseGameplayAbility>> AbilitiesToGrant;

	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct FBluehorseHeroWeaponData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBluehorseHeroLinkedAnimLayer> WeaponAnimLayerToLink;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FBluehorseHeroAbilitySet> DefaultWeaponAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> SoftWeaponIconTexture;
};