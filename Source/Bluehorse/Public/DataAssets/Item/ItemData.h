// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraSystem.h"   
#include "ItemData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Consumable,
	Equipment,
	Material
};

class UGameplayEffect;

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStack = 99;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSubclassOf<UGameplayEffect> EffectToApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 ApplyLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UNiagaraSystem> UseEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<USoundBase> UseSound = nullptr;

private:
	UFUNCTION(BlueprintCallable, Category = "Item")
	UNiagaraSystem* GetUseEffect();

	UFUNCTION(BlueprintCallable, Category = "Item")
	USoundBase* GetUseSound();
};
