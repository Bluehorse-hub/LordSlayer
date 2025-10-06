// Shungen All Rights Reserved.


#include "BluehorseGameInstance.h"
#include "Kismet/GameplayStatics.h"

#include "BluehorseDebugHelper.h"

TSoftObjectPtr<UWorld> UBluehorseGameInstance::GetGameLevelByTag(FGameplayTag InTag) const
{
    for (const FBluehorseGameLevelSet& GameLevelSet : GameLevelSets)
    {
        if (!GameLevelSet.IsValid()) continue;

        if (GameLevelSet.LevelTag == InTag)
        {
            return GameLevelSet.Level;
        }
    }

    return TSoftObjectPtr<UWorld>();
}

USoundBase* UBluehorseGameInstance::GetBGMByLevelTag(FGameplayTag InTag) const
{
    for (const FBluehorseGameLevelSet& GameLevelSet : GameLevelSets)
    {
        if (!GameLevelSet.IsValid()) continue;

        if (GameLevelSet.LevelTag == InTag)
        {
            Debug::Print(TEXT("Get GameLevelSet.BGM"), FColor::Green);
            return GameLevelSet.BGM.LoadSynchronous();
        }
    }

    Debug::Print(TEXT("Can NOT Get GameLevelSet.BGM"), FColor::Red);
    return nullptr;
}

void UBluehorseGameInstance::PlayBGM(USoundBase* NewBGM, float FadeOutTime, float FadeInTime)
{
    if (!NewBGM)
    {
        Debug::Print(TEXT("NewBGM is NULL"), FColor::Red);
        return;
    }

    if (CurrentSound == NewBGM && CurrentBGM && CurrentBGM->IsPlaying())
    {
        Debug::Print(TEXT("CurrentSound is now ringing"), FColor::Red);
        return;
    }

    if (CurrentBGM)
    {
        CurrentBGM->FadeOut(FadeOutTime, 0.f);
        CurrentBGM = nullptr;
    }

    CurrentBGM = UGameplayStatics::SpawnSound2D(this, NewBGM, 1.f, 1.f, 0.f, nullptr, true);
    if (CurrentBGM)
    {
        Debug::Print(TEXT("CurrentBGM is NOT NULL"), FColor::Green);
        CurrentBGM->FadeIn(FadeInTime, 1.f);
        CurrentSound = NewBGM;
    }
    else
    {
        Debug::Print(TEXT("CurrentBGM is NULL"), FColor::Red);
    }
}
