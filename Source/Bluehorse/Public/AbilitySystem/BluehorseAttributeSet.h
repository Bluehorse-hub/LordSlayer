// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "BluehorseAttributeSet.generated.h"

class IPawnUIInterface;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBluehorseAttributeSet();

	// GameplayEffect��Attribute�ɔ��f���ꂽ�����AttributeSet���ŌĂ΂�鉼�z�֐�
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UBluehorseAttributeSet, CurrentHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBluehorseAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "SkillPoint")
	FGameplayAttributeData CurrentSkillPoint;
	ATTRIBUTE_ACCESSORS(UBluehorseAttributeSet, CurrentSkillPoint)

	UPROPERTY(BlueprintReadOnly, Category = "SkillPoint")
	FGameplayAttributeData MaxSkillPoint;
	ATTRIBUTE_ACCESSORS(UBluehorseAttributeSet, MaxSkillPoint)

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	FGameplayAttributeData CurrentStamina;
	ATTRIBUTE_ACCESSORS(UBluehorseAttributeSet, CurrentStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UBluehorseAttributeSet, MaxStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UBluehorseAttributeSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UBluehorseAttributeSet, DefensePower)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UBluehorseAttributeSet, DamageTaken)

private:
	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;
};
