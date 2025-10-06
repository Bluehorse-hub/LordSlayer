// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UBluehorseGameplayAbility;
class UBluehorseAbilitySystemComponent;
class UGameplayEffect;


/**
 * 
 */

//--- 初期アビリティを付与する基本クラス ---//
UCLASS()
class BLUEHORSE_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UBluehorseAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UBluehorseGameplayAbility > > ActivateOnGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UBluehorseGameplayAbility > > ReactiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UGameplayEffect > > StartUpGameplayEffects;

	void GrantAbilities(const TArray< TSubclassOf < UBluehorseGameplayAbility > >& InAbilitiesToGive, UBluehorseAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);
};
