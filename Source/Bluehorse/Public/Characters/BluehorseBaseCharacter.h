// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Interfaces/PawnUIInterface.h"
#include "Interfaces/PawnHitEffectInterface.h"
#include "BluehorseBaseCharacter.generated.h"

class UBluehorseAbilitySystemComponent;
class UBluehorseAttributeSet;
class UDataAsset_StartUpDataBase;
class UMotionWarpingComponent;

UCLASS()
class BLUEHORSE_API ABluehorseBaseCharacter : public ACharacter, public IAbilitySystemInterface, public IPawnCombatInterface, public IPawnUIInterface, public IPawnHitEffectInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABluehorseBaseCharacter();

	//~ Begin IAbilitySystemInterface Interface.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface

	//~ Begin IPawnCombatInterface Interface.
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	//~ End IPawnCombatInterface Interface

	//~ Begin IPawnUIInterface Interface.
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	//~ End IPawnUIInterface Interface

	//~ Begin IPawnHitEffectInterface Interface.
	virtual UNiagaraSystem* GetHitEffect() const override;
	virtual USoundBase* GetHitSound() const override;
	//~ End IPawnHitEffectInterface Interface

protected:
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UBluehorseAbilitySystemComponent* BluehorseAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UBluehorseAttributeSet* BluehorseAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarping")
	UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit Effect")
	UNiagaraSystem* DefaultHitEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit Effect")
	USoundBase* DefaultHitSound;

public:
	FORCEINLINE UBluehorseAbilitySystemComponent* GetBluehorseAbilitySystemComponent() const { return BluehorseAbilitySystemComponent;}

	FORCEINLINE UBluehorseAttributeSet* GetBluehorseAttributeSet() const {return BluehorseAttributeSet;}
};
