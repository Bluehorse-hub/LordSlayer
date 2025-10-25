// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/BluehorseHeroGameplayAbility.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Controllers/BluehorseHeroController.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "BluehorseGamePlayTags.h"

// GAS�ŃA�r���e�B����Ή�����L�����N�^�[���擾����֐�
ABluehorseHeroCharacter* UBluehorseHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	// �L���b�V���������Ȃ�Ď擾�i�p�t�H�[�}���X����̂���WeakPtr�g�p�j
	if (!CachedBluehorseHeroCharacter.IsValid())
	{
		CachedBluehorseHeroCharacter = Cast<ABluehorseHeroCharacter>(CurrentActorInfo->AvatarActor);
	}

	// �L���b�V�����L���Ȃ�Ԃ��A�����Ȃ�nullptr
	return CachedBluehorseHeroCharacter.IsValid() ? CachedBluehorseHeroCharacter.Get() : nullptr;
}

ABluehorseHeroController* UBluehorseHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	if (!CachedBluehorseHeroController.IsValid())
	{
		CachedBluehorseHeroController = Cast<ABluehorseHeroController>(CurrentActorInfo->PlayerController);
	}

	return CachedBluehorseHeroController.IsValid() ? CachedBluehorseHeroController.Get() : nullptr;
}

UHeroCombatComponent* UBluehorseHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

// �_���[�W�p��GameplayEffectSpecHandle�𐶐�
FGameplayEffectSpecHandle UBluehorseHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, float MotionValue)
{
	
	check(EffectClass);

	// EffectContext���쐬
	FGameplayEffectContextHandle ContextHandle = GetBluehorseAbilitySystemComponentFromActorInfo()->MakeEffectContext();

	// ���̃A�r���e�B���̂�EffectContext�ɓo�^�i���������Ƃ��āj
	ContextHandle.SetAbility(this);

	// �U���ҁiSourceObject, Instigator�j��o�^
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	// GameplayEffectSpec�𐶐�
	// �w�肵��EffectClass�����ɁA�ŏI�I�ȃ_���[�W�v�Z�Ɏg�p�����Spec�����
	FGameplayEffectSpecHandle EffectSpecHandle = GetBluehorseAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	// SetByCaller���g�p���ĊO���p�����[�^��ݒ�
	// �U�����̊�{�_���[�W�� "Shared.SetByCaller.BaseDamage" �ɃZ�b�g
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		BluehorseGameplayTags::Shared_SetByCaller_BaseDamage,
		InWeaponBaseDamage
	);

	// �U����ʁiLight/Heavy�Ȃǁj���L���Ȃ�A�����SetByCaller�œn��
	// GEExecCalc_DamageTaken�̒���Tag�ɉ������{����^����
	if (InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, MotionValue);
	}

	// �쐬�ς݂�EffectSpec��Ԃ�
	return EffectSpecHandle;
}
