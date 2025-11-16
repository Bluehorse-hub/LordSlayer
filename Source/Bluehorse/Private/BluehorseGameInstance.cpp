// Shungen All Rights Reserved.


#include "BluehorseGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MoviePlayer.h"

#include "BluehorseDebugHelper.h"


// 指定されたLevelTagに対応するレベル(World)を取得
TSoftObjectPtr<UWorld> UBluehorseGameInstance::GetGameLevelByTag(FGameplayTag InTag) const
{
    for (const FBluehorseGameLevelSet& GameLevelSet : GameLevelSets)
    {
        // 無効なレベルデータはスキップ
        if (!GameLevelSet.IsValid()) continue;

        // タグが一致した場合、そのレベルを返す
        if (GameLevelSet.LevelTag == InTag)
        {
            return GameLevelSet.Level;
        }
    }

    // 見つからなかった場合は空のTSoftObjectPtrを返す
    return TSoftObjectPtr<UWorld>();
}

// 指定されたLevelTagに対応するBGM(SoundBase)を取得
USoundBase* UBluehorseGameInstance::GetBGMByLevelTag(FGameplayTag InTag) const
{
    for (const FBluehorseGameLevelSet& GameLevelSet : GameLevelSets)
    {
        if (!GameLevelSet.IsValid()) continue;

        if (GameLevelSet.LevelTag == InTag)
        {
            // ロード時に同期読み込み（非同期にする場合は別処理）
            //Debug::Print(TEXT("Get GameLevelSet.BGM"), FColor::Green);
            return GameLevelSet.BGM.LoadSynchronous();
        }
    }

    //Debug::Print(TEXT("Can NOT Get GameLevelSet.BGM"), FColor::Red);
    return nullptr;
}

// BGMを再生・フェード制御付きで切り替える
void UBluehorseGameInstance::PlayBGM(USoundBase* NewBGM, float FadeOutTime, float FadeInTime)
{
    if (!NewBGM)
    {
        //Debug::Print(TEXT("NewBGM is NULL"), FColor::Red);
        return;
    }

    // すでに同じBGMが再生中の場合は何もしない
    if (CurrentSound == NewBGM && CurrentBGM && CurrentBGM->IsPlaying())
    {
        //Debug::Print(TEXT("CurrentSound is now ringing"), FColor::Red);
        return;
    }

    // 現在のBGMが存在する場合はフェードアウトして停止
    if (CurrentBGM)
    {
        CurrentBGM->FadeOut(FadeOutTime, 0.f);
        CurrentBGM = nullptr;
    }

    // 新しいBGMを2Dサウンドとして生成
    CurrentBGM = UGameplayStatics::SpawnSound2D(this, NewBGM, 1.f, 1.f, 0.f, nullptr, false);
    if (CurrentBGM)
    {
        // 新BGMをフェードイン再生
        //Debug::Print(TEXT("CurrentBGM is NOT NULL"), FColor::Green);
        CurrentBGM->FadeIn(FadeInTime, 1.f);

        CurrentBGM->bAutoDestroy = false;

        CurrentSound = NewBGM;
    }
    else
    {
        //Debug::Print(TEXT("CurrentBGM is NULL"), FColor::Red);
    }
}

// レベル遷移時に呼ばれ、現在のLevelTagを更新
void UBluehorseGameInstance::NotifyLevelTransitionByTag(FGameplayTag NewLevelTag)
{
    // 初回起動時は遷移扱いしない
    if (!CurrentLevelTag.IsValid())
    {
        bHasTransitionedFromOtherLevel = false;
    }
    // 現在と異なるレベルタグに切り替わった場合のみ「遷移あり」
    else if (CurrentLevelTag != NewLevelTag)
    {
        bHasTransitionedFromOtherLevel = true;
    }
    else
    {
        bHasTransitionedFromOtherLevel = false;
    }

    // 遷移履歴を更新
    LastLevelTag = CurrentLevelTag;
    CurrentLevelTag = NewLevelTag;

    UE_LOG(LogTemp, Warning, TEXT("[GameInstance] Level transition: %s to %s (HasTransitioned=%s)"),
        *LastLevelTag.ToString(),
        *CurrentLevelTag.ToString(),
        bHasTransitionedFromOtherLevel ? TEXT("True") : TEXT("False"));
}

// 前のレベルから遷移してきたかどうかを返す
bool UBluehorseGameInstance::HasTransitionedFromOtherLevel() const
{
    return bHasTransitionedFromOtherLevel;
}

// 現在のレベルが戦闘エリアかどうかを判定
bool UBluehorseGameInstance::IsCurrentLevelCombatArea() const
{
    for (const FBluehorseGameLevelSet& LevelSet : GameLevelSets)
    {
        if (LevelSet.LevelTag == CurrentLevelTag)
        {
            return LevelSet.bIsCombatArea;
        }
    }
    return false;
}

// PreLoadMapが呼ばれたときに実行される
void UBluehorseGameInstance::OnPreLoadMap(const FString& MapName)
{
    // ローディング画面用の設定情報を格納する構造体
    FLoadingScreenAttributes LoadingScreenAttributes;

    // マップロード完了時に、自動でローディング画面を閉じる
    LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;

    // ローディング画面を最低2秒は表示する（ロードが速すぎても一瞬で消えないようにする）
    LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 2.f;

    // 表示するローディング用ウィジェット
    // デフォルトで用意されているものを使う
    LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

    // ローディング画面の表示をMoviePlayerに登録
    GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

// 新しいワールドがロード完了したときに呼ばれる
void UBluehorseGameInstance::OnDestinationWorldLoaded(UWorld* LoadedWorld)
{
    // ローディング画面（ムービー含む）を停止する
    GetMoviePlayer()->StopMovie();
}

// GameInstance初期化時に現在のマップを解析し、対応するLevelTagを検出
void UBluehorseGameInstance::Init()
{
    Super::Init();

    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::OnPreLoadMap);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnDestinationWorldLoaded);

    // 現在のマップ名を取得
    const FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(this, true);
    FGameplayTag DetectedTag;

    // ---------------------------------------------------------------
    // 現在ロードされたマップ名と GameLevelSets に登録された LevelAsset を照合し、
    // 対応する GameplayTag（LevelTag）を特定する処理。
    // ※ 現状は GameLevelSets によるマップ管理と、
    //    ローディング画面/レベル遷移管理が別々に動いているため、
    //    将来的には本処理もレベル遷移管理へ統合する余地がある。
    // ---------------------------------------------------------------
    for (const FBluehorseGameLevelSet& LevelSet : GameLevelSets)
    {
        if (LevelSet.Level.IsValid())
        {
            FString LevelAssetName = LevelSet.Level.GetAssetName();

            // 現在のマップ名に一致するLevelAssetを発見
            if (CurrentMapName.Contains(LevelAssetName))
            {
                DetectedTag = LevelSet.LevelTag;
                break;
            }
        }
    }

    // ---------------------------------------------------------------
    // マッチする LevelTag が見つかった場合は、
    // "現在のレベルタグ" として登録し、遷移情報を更新。
    // ※ ここで設定される CurrentLevelTag は、マップロード後の初期化動作に使用可能。
    // ---------------------------------------------------------------
    if (DetectedTag.IsValid())
    {
        CurrentLevelTag = DetectedTag;
        LastLevelTag = FGameplayTag();
        bHasTransitionedFromOtherLevel = false;

        UE_LOG(LogTemp, Warning, TEXT("[GameInstance] Initial Map Detected: %s"), *CurrentLevelTag.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameInstance] No matching LevelTag found for map: %s"), *CurrentMapName);
    }
}
