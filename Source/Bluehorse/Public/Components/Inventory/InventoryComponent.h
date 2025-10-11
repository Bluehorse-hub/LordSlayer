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

USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Item"))
    FGameplayTag ItemTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 0;

    UPROPERTY()
    FPrimaryAssetId ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UItemData> ItemData = nullptr;
};

UCLASS()
class BLUEHORSE_API UInventoryComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
    UInventoryComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetItemCount(FGameplayTag ItemTag) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddItem(FGameplayTag ItemTag, int32 Amount, UItemData* ItemData);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UseItemByTag(FGameplayTag ItemTag, UBluehorseAbilitySystemComponent* AbilitySystemComponent, bool& bSlotRemoved);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UseItemByIndex(UBluehorseAbilitySystemComponent* AbilitySystemComponent, bool& bSlotRemoved);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SelectItem();

    UFUNCTION(BlueprintPure, Category = "Inventory")
    FInventorySlot GetCurrentItem();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void PlayUseItemSound(USoundBase* SoundToPlay);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SaveInventoryToGameInstance();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void LoadInventoryFromGameInstance();



    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int MaxInventorySlot;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<FInventorySlot> InventorySlots;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
    UDataTable* ItemDataTable;

private:
    FInventorySlot* FindSlot(FGameplayTag ItemTag);

    const FInventorySlot* ConstFindSlot(FGameplayTag ItemTag) const;

    UItemData* GetItemDataFromTag(const FGameplayTag& ItemTag) const;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    int CurrentInventoryIndex = 0;

    void LogInventory() const;

};
