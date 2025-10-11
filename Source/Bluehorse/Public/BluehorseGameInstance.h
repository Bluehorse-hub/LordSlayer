// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameplayTagContainer.h"
#include "Components/AudioComponent.h"
#include "BluehorseGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FBluehorseGameLevelSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "GameData.Level"))
	FGameplayTag LevelTag;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> Level;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USoundBase> BGM;

	bool IsValid() const
	{
		return LevelTag.IsValid() && !Level.IsNull();
	}
};

// ���x���Ԏ����z���p�̌y�ʃA�C�e�����
USTRUCT(BlueprintType)
struct FStoredItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Count = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 SlotIndex = -1;
};

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//--- ���x�� ---//
	UFUNCTION(BlueprintPure, meta = (GameplayTagFilter = "GameData.Level"))
	TSoftObjectPtr<UWorld> GetGameLevelByTag(FGameplayTag InTag) const;

	UFUNCTION(BlueprintPure, meta = (GameplayTagFilter = "GameData.Level"))
	USoundBase* GetBGMByLevelTag(FGameplayTag InTag) const;

	UFUNCTION(BlueprintCallable)
	void PlayBGM(USoundBase* NewBGM, float FadeOutTime = 1.0f, float FadeInTime = 1.0f);

	UPROPERTY(BlueprintReadWrite, Category = "Level Transition")
	bool bHasTransitionedFromOtherLevel = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	FGameplayTag CurrentLevelTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	FGameplayTag LastLevelTag;

	UFUNCTION(BlueprintCallable, Category = "Level")
	void NotifyLevelTransitionByTag(FGameplayTag NewLevelTag);

	UFUNCTION(BlueprintCallable, Category = "Level")
	bool HasTransitionedFromOtherLevel() const;

	//--- �A�C�e�� ---//
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TArray<FStoredItemData> PersistentInventory;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FBluehorseGameLevelSet> GameLevelSets;

	UPROPERTY()
	UAudioComponent* CurrentBGM;

	UPROPERTY()
	USoundBase* CurrentSound;
};
