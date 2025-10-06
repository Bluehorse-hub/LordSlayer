// Shungen All Rights Reserved.


#include "Components/Combat/PawnCombatComponent.h"
#include "Items/Weapons/BluehorseWeaponBase.h"
#include "Components/BoxComponent.h"

#include "BluehorseDebugHelper.h"

// •Ší‚ðTag‚ÆƒyƒA‚É‚µ‚Ä“o˜^‚·‚éŠÖ”
void UPawnCombatComponent::RegisterSpawnWeapon(FGameplayTag InWeaponTagToRegister, ABluehorseWeaponBase* InWeaponToRegister, EWeaponSlot Slot)
{
	// Tag‚Ìd•¡“o˜^‚ð–h‚®
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A named named %s has already been added as carried weapon"), *InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);

	// “o˜^ˆ—
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	// SpawnŽž‚É“–‚½‚è”»’èŒn‚ÌŠÖ”‚ðƒoƒCƒ“ƒh‚µ‚Ä‚¨‚­
	InWeaponToRegister->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
	InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);

	// SpawnŽž‚É¶‰E‚Å“o˜^‚·‚éˆ—(BP‘¤‚Å“o˜^‚·‚é‚±‚Æ‚ª‘½‚¢‚Ì‚ÅƒTƒu“I‚È‹@”\)
	switch (Slot)
	{
	case EWeaponSlot::PreEquip:
		break;
	case EWeaponSlot::LeftHand:
		EquippedLeftHandWeaponTag = InWeaponTagToRegister;
		break;
	case EWeaponSlot::RightHand:
		EquippedRightHandWeaponTag = InWeaponTagToRegister;
		break;
	default:
		break;
	}

	/*UE_LOG(LogTemp, Warning, TEXT("[RegisterSpawnWeapon] Tag: %s | Slot: %s | Weapon: %s"),
		*InWeaponTagToRegister.ToString(),
		*UEnum::GetValueAsString(Slot),
		*InWeaponToRegister->GetName());*/
}

// Tag‚©‚ç•Ší‚ðŽæ“¾‚·‚éŠÖ”
ABluehorseWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		if (ABluehorseWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			return *FoundWeapon;
		}
	}

	return nullptr;
}

ABluehorseWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}

	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

ABluehorseWeaponBase* UPawnCombatComponent::GetCharacterEquippedRightHandWeapon() const
{
	if (!EquippedRightHandWeaponTag.IsValid())
	{
		return nullptr;
	}

	return GetCharacterCarriedWeaponByTag(EquippedRightHandWeaponTag);
}

ABluehorseWeaponBase* UPawnCombatComponent::GetCharacterEquippedLeftHandWeapon() const
{
	if (!EquippedLeftHandWeaponTag.IsValid())
	{
		return nullptr;
	}

	return GetCharacterCarriedWeaponByTag(EquippedLeftHandWeaponTag);
}

void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	if (ToggleDamageType == EToggleDamageType::EquippedRightHandWeapon || ToggleDamageType == EToggleDamageType::EquippedLeftHandWeapon)
	{

		ABluehorseWeaponBase* WeaponToToggle;

		if (ToggleDamageType == EToggleDamageType::EquippedRightHandWeapon)
		{
			WeaponToToggle = GetCharacterEquippedRightHandWeapon();
		}
		else
		{
			WeaponToToggle = GetCharacterEquippedLeftHandWeapon();
		}

		check(WeaponToToggle);

		if (bShouldEnable)
		{
			WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else
		{
			WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			// UŒ‚I—¹Žž‚É‚Í‚»‚ê‚Ü‚Å‚Ìƒqƒbƒg‚µ‚½‘ŠŽè‚Ì‹L˜^‚ð”jŠü‚·‚é
			OverlappedActors.Empty();
			WeaponToToggle->IgnoreActors.Empty();
		}
	}

	// TODO: Handle body collision box
}

void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult)
{
}

void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}


