// Shungen All Rights Reserved.


#include "Characters/BluehorseBaseCharacter.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "AbilitySystem/BluehorseAttributeSet.h"
#include "MotionWarpingComponent.h"

// Sets default values
ABluehorseBaseCharacter::ABluehorseBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;

	BluehorseAbilitySystemComponent = CreateDefaultSubobject<UBluehorseAbilitySystemComponent>(TEXT("BluehorseAbilitySystemComponent"));

	BluehorseAttributeSet = CreateDefaultSubobject<UBluehorseAttributeSet>(TEXT("BluehorseAttributeSet"));

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

UAbilitySystemComponent* ABluehorseBaseCharacter::GetAbilitySystemComponent() const
{
	return GetBluehorseAbilitySystemComponent();
}

UPawnCombatComponent* ABluehorseBaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

UPawnUIComponent* ABluehorseBaseCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

UNiagaraSystem* ABluehorseBaseCharacter::GetHitEffect() const
{
	return DefaultHitEffect;
}

USoundBase* ABluehorseBaseCharacter::GetHitSound() const
{
	return DefaultHitSound;
}

void ABluehorseBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// GAS‚Ì‰Šú‰»ˆ—
	if (BluehorseAbilitySystemComponent)
	{
		BluehorseAbilitySystemComponent->InitAbilityActorInfo(this, this);

		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());
	}
}

