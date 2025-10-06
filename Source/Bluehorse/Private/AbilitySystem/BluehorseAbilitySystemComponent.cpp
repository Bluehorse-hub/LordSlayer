// Shungen All Rights Reserved.


#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/BluehorseGameplayAbility.h"
#include "BluehorseGameplayTags.h"

#include "BluehorseDebugHelper.h"

// “Á’è‚Ì“ü—ÍTag‚É‘Î‚µ‚Ä‚»‚ê‚É•R‚Ã‚¢‚½GameAbility‚ð’T‚µ‚Ä”­“®‚·‚éŠÖ”
void UBluehorseAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;

		if (InInputTag.MatchesTag(BluehorseGameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
			TryActivateAbility(AbilitySpec.Handle);
		}
	}

	/*FGameplayTagContainer GameplayTagContainer;
	GameplayTagContainer.AddTag(InInputTag);
	const bool bAny = TryActivateAbilitiesByTag(GameplayTagContainer, true);
	UE_LOG(LogTemp, Log, TEXT("TryActivateAbilitiesByTag -> %s"), bAny ? TEXT("true") : TEXT("false"));*/
}

void UBluehorseAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	// Debug::Print(TEXT("Input Released Activate"), FColor::Green, 1);

	if (!InInputTag.IsValid())
	{
		// Debug::Print(TEXT("Is NOT Release target"), FColor::Red, 2);
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		const bool bMustBeHeld = AbilitySpec.DynamicAbilityTags.HasTagExact(BluehorseGameplayTags::Player_Ability_MustBeHeld) ||
			(AbilitySpec.Ability && AbilitySpec.Ability->AbilityTags.HasTagExact(BluehorseGameplayTags::Player_Ability_MustBeHeld));

		if (bMustBeHeld && AbilitySpec.IsActive())
		{
			// Debug::Print(TEXT("Cancel Ability"), FColor::Cyan, 4);
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

void UBluehorseAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FBluehorseHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	if (InDefaultWeaponAbilities.IsEmpty())
	{
		return;
	}

	for (const FBluehorseHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;

		for (const TSubclassOf<UBluehorseGameplayAbility>& AbilityClass : AbilitySet.AbilitiesToGrant)
		{
			if (!*AbilityClass) continue;

			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;
			AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

			OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
		}
	}
}

void UBluehorseAbilitySystemComponent::RemoveGtantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}

	InSpecHandlesToRemove.Empty();
}

bool UBluehorseAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);

		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}

	return false;
}
