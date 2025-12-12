// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "HajikiAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UHajikiAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UHajikiAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Hajiki")
	FGameplayAttributeData HajikiAnimIndex;
	ATTRIBUTE_ACCESSORS(UHajikiAttributeSet, HajikiAnimIndex)

	UFUNCTION()
	int32 GetCurrentHajikiAnimIndex() const;

	UFUNCTION()
	void SetCurrentHajikiAnimIndex(int32 InIndex);

	UPROPERTY(BlueprintReadOnly, Category = "Hajiki")
	FGameplayAttributeData HajikiCount;
	ATTRIBUTE_ACCESSORS(UHajikiAttributeSet, HajikiCount)

	// Optional: ç°å„ï÷óòÇ»ä÷êî
	UFUNCTION(BlueprintCallable, Category = "Hajiki")
	int32 GetCurrentHajikiCount();

	UFUNCTION(BlueprintCallable, Category = "Hajiki")
	void AddHajikiCount(int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Hajiki")
	void ResetHajikiCount();
};
