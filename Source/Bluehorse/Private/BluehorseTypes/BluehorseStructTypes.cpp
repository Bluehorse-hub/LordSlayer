// Shungen All Rights Reserved.


#include "BluehorseTypes/BluehorseStructTypes.h"
#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"

bool FBluehorseHeroAbilitySet::IsValid() const
{
	for (const auto& Ability : AbilitiesToGrant)
	{
		if (Ability == nullptr)
		{
			return false;
		}
	}

	return InputTag.IsValid();
}
