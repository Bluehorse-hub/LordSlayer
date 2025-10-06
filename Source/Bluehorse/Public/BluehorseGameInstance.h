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

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (GameplayTagFilter = "GameData.Level"))
	TSoftObjectPtr<UWorld> GetGameLevelByTag(FGameplayTag InTag) const;

	UFUNCTION(BlueprintPure, meta = (GameplayTagFilter = "GameData.Level"))
	USoundBase* GetBGMByLevelTag(FGameplayTag InTag) const;

	UFUNCTION(BlueprintCallable)
	void PlayBGM(USoundBase* NewBGM, float FadeOutTime = 1.0f, float FadeInTime = 1.0f);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FBluehorseGameLevelSet> GameLevelSets;

	UPROPERTY()
	UAudioComponent* CurrentBGM;

	UPROPERTY()
	USoundBase* CurrentSound;
	
};
