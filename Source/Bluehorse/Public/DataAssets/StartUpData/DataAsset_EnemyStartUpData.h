// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "GameplayTagContainer.h"
#include "DataAsset_EnemyStartUpData.generated.h"

class UBluehorseEnemyGameplayAbility;

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UDataAsset_EnemyStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyType", meta = (Categories = "Enemy.Type"))
	FGameplayTag EnemyTypeTag;

	virtual void GiveToAbilitySystemComponent(UBluehorseAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UBluehorseEnemyGameplayAbility>> EnemyCombatAbilities;
};
