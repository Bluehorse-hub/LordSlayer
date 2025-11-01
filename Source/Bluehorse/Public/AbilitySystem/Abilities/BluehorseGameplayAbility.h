// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BluehorseTypes/BluehorseEnumTypes.h"
#include "BluehorseGameplayAbility.generated.h"

class UPawnCombatComponent;
class UBluehorseAbilitySystemComponent;

UENUM(BlueprintType)
enum class EBluehorseAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};


/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	//~ Begin UGameplayAbility Interface.
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface.

	UPROPERTY(EditDefaultsOnly, Category = "Bluehorse|Ability")
	EBluehorseAbilityActivationPolicy AbilityActivationPolicy = EBluehorseAbilityActivationPolicy::OnTriggered;

	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Bluehorse|Ability")
	UBluehorseAbilitySystemComponent* GetBluehorseAbilitySystemComponentFromActorInfo() const;

	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Ability", meta = (DisplayName = "Apply Gameplay Effect Spec Handle To Target Actor", ExpandEnumAsExecs = "OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EBluehorseSuccessType& OutSuccessType);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|Ability")
	void ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults);
};
