// Shungen All Rights Reserved.


#include "Components/Combat/PawnCombatComponent.h"
#include "Items/Weapons/BluehorseWeaponBase.h"
#include "Components/BoxComponent.h"

#include "BluehorseDebugHelper.h"

// �����Tag�ƃy�A�ɂ��ēo�^����֐�
void UPawnCombatComponent::RegisterSpawnWeapon(FGameplayTag InWeaponTagToRegister, ABluehorseWeaponBase* InWeaponToRegister, EWeaponSlot Slot)
{
	// Tag�̏d���o�^��h��
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A named named %s has already been added as carried weapon"), *InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);

	// �o�^����
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	// Spawn���ɓ����蔻��n�̊֐����o�C���h���Ă���
	InWeaponToRegister->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
	InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);

	// Spawn���ɍ��E�œo�^���鏈��(BP���œo�^���邱�Ƃ������̂ŃT�u�I�ȋ@�\)
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

// Tag���畐����擾����֐�
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

			// �U���I�����ɂ͂���܂ł̃q�b�g��������̋L�^��j������
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


