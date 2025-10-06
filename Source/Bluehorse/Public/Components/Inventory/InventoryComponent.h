// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"
#include "InventoryComponent.generated.h"

/**
 * 
 */

class UItemData;
class UBluehorseAbilitySystemComponent;

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Consumable,
    Equipment,
    Material
};

USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Item"))
    FGameplayTag ItemTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxStack = 99;

    UPROPERTY()
    FPrimaryAssetId ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<class UItemData> ItemData = nullptr;
};

UCLASS()
class BLUEHORSE_API UInventoryComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
    UInventoryComponent();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetItemCount(FGameplayTag ItemTag) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddItem(FGameplayTag ItemTag, int32 Amount, EItemType ItemType, int32 MaxStack, UItemData* ItemData);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UseItemByTag(FGameplayTag ItemTag, UBluehorseAbilitySystemComponent* AbilitySystemComponent, bool& bSlotRemoved);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UseItemByIndex(UBluehorseAbilitySystemComponent* AbilitySystemComponent, bool& bSlotRemoved);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SelectItem();

    UFUNCTION(BlueprintPure, Category = "Inventory")
    FInventorySlot& GetCurrentItem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int MaxInventorySlot = 4;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<FInventorySlot> InventorySlots;

private:
    FInventorySlot* FindSlot(FGameplayTag ItemTag);

    const FInventorySlot* ConstFindSlot(FGameplayTag ItemTag) const;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    int CurrentInventoryIndex = 0;

    void LogInventory() const;

};
