// Shungen All Rights Reserved.


#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"


void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UBluehorseAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
    Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

    for (const FBluehorseHeroAbilitySet& AbilitySet : HeroStartUpAbilitySets)
    {
        if (!AbilitySet.IsValid()) continue;

        for (const TSubclassOf<UBluehorseGameplayAbility>& AbilityClass : AbilitySet.AbilitiesToGrant)
        {
            if (!*AbilityClass) continue;

            FGameplayAbilitySpec AbilitySpec(AbilityClass);
            AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
            AbilitySpec.Level = ApplyLevel;
            AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

            InASCToGive->GiveAbility(AbilitySpec);
        }
    }
}