// Shungen All Rights Reserved.


#include "BluehorseFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BluehorseGameplayTags.h"
#include "BluehorseGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "DefaultLevelSequenceInstanceData.h"

#include "BluehorseDebugHelper.h"

/**
 * 指定した Actor から Bluehorse 用 AbilitySystemComponent を取得する。
 *
 * - GAS を利用しているかどうかを安全に判定するための共通関数
 *
 * @param InActor ASC を取得したい Actor
 * @return BluehorseAbilitySystemComponent（存在しない場合は nullptr）
 */
UBluehorseAbilitySystemComponent* UBluehorseFunctionLibrary::NativeGetBluehorseASCFromActor(AActor* InActor)
{
    // Actor が無効な場合は取得不可
    if (!IsValid(InActor)) return nullptr;

    // GAS 標準関数から ASC を取得し、Bluehorse 用にキャスト
    return Cast<UBluehorseAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

/**
 * Actor が指定した GameplayTag を持っていない場合のみ追加する。
 *
 * - LooseGameplayTag を使用する（GameplayEffect 由来ではない状態用）
 * - 既に Tag を持っている場合は何もしない
 *
 * @param InActor Tag を付与する対象 Actor
 * @param TagToAdd 追加したい GameplayTag
 */
void UBluehorseFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
    if (!TagToAdd.IsValid()) return;

    UBluehorseAbilitySystemComponent* ASC = NativeGetBluehorseASCFromActor(InActor);

    // 既に Tag を持っていなければ追加
    if (!ASC->HasMatchingGameplayTag(TagToAdd))
    {
        ASC->AddLooseGameplayTag(TagToAdd);
    }
}

/**
 * Actor が指定した GameplayTag を持っている場合のみ削除する。
 *
 * - LooseGameplayTag を対象とする
 * - Tag を持っていない場合は安全に何もしない
 *
 * @param InActor Tag を削除する対象 Actor
 * @param TagToRemove 削除したい GameplayTag
 */
void UBluehorseFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
    if (!TagToRemove.IsValid()) return;

    UBluehorseAbilitySystemComponent* ASC = NativeGetBluehorseASCFromActor(InActor);

    if (ASC->HasMatchingGameplayTag(TagToRemove))
    {
        ASC->RemoveLooseGameplayTag(TagToRemove);
    }
}

/**
 * Actor が指定した GameplayTag を保持しているかを判定する。
 *
 * - ASC を持たない Actor に対しても安全に false を返す
 * - UI / Anim / Weapon / AOE など幅広い箇所で使用される想定
 *
 * @param InActor 判定対象の Actor
 * @param TagToCheck チェックする GameplayTag
 * @return Tag を保持していれば true
 */
bool UBluehorseFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
    if (!IsValid(InActor)) return false;

    UBluehorseAbilitySystemComponent* ASC = NativeGetBluehorseASCFromActor(InActor);

    if (!IsValid(ASC)) return false;

    return ASC->HasMatchingGameplayTag(TagToCheck);
}

/**
 * Blueprint 用：Actor が GameplayTag を持っているかを判定し、
 * Branch ノードに繋ぎやすい ConfirmType（Yes/No）として返す。
 *
 * - 実体の判定は NativeDoesActorHaveTag に委譲する
 * - BP で bool ではなく enum（Exec/分岐）を使いたい時のユーティリティ
 *
 * @param InActor 判定対象の Actor
 * @param TagToCheck チェックする GameplayTag
 * @param OutConfirmType Yes/No を返す（BP分岐用）
 */
void UBluehorseFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EBluehorseConfirmType& OutConfirmType)
{
    OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EBluehorseConfirmType::Yes : EBluehorseConfirmType::No;
}

/**
 * Actor から PawnCombatComponent を取得する（C++用）。
 *
 * - Actor が IPawnCombatInterface を実装している場合のみ取得可能
 * - Character などの具体クラスに依存せず、Interface経由で疎結合に扱う
 *
 * @param InActor CombatComponent を持つ可能性のある Actor
 * @return CombatComponent（取得できない場合は nullptr）
 */
UPawnCombatComponent* UBluehorseFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
    // Actor が無効なら取得不可
    if (!IsValid(InActor))
    {
        return nullptr;
    }

    // CombatInterface を実装している Actor から Component を取得
    if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
    {
        return PawnCombatInterface->GetPawnCombatComponent();
    }

    return nullptr;
}

/**
 * Blueprint 用：Actor から PawnCombatComponent を取得し、
 * 有効/無効を enum（Valid/Invalid）で返す。
 *
 * - BP で「取得できたか」を分岐しやすくするためのラッパー
 * - 実体の取得は NativeGetPawnCombatComponentFromActor に委譲する
 *
 * @param InActor CombatComponent を取得したい Actor
 * @param OutValidType Valid / Invalid を返す（BP分岐用）
 * @return CombatComponent（Invalid の場合は nullptr）
 */
UPawnCombatComponent* UBluehorseFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor, EBluehorseValidType& OutValidType)
{
    UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

    OutValidType = CombatComponent ? EBluehorseValidType::Valid : EBluehorseValidType::Invalid;
    return CombatComponent;
}

/**
 * QueryPawn と TargetPawn が敵対関係かどうかを判定する。
 *
 * 目的：
 * - 敵AIの攻撃で「敵同士が殴り合う（Friendly Fire）」を防ぐ
 * - Weapon / Projectile / AOE など、攻撃判定の共通フィルタとして使用する
 *
 * 判定方法：
 * - 両者の Controller が IGenericTeamAgentInterface を実装していることを前提に、
 *   GenericTeamId が異なれば敵対（Hostile）とみなす
 *
 * @param QueryPawn 攻撃/判定の主体（例：武器の Instigator）
 * @param TargetPawn 攻撃対象候補
 * @return 敵対している場合 true（同じチーム、または情報不足なら false）
 */
bool UBluehorseFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
    // nullptr 安全：情報不足なら敵対扱いにしない
    if (!QueryPawn || !TargetPawn)
    {
        return false;
    }

    // 両者の Controller から TeamAgent を取得
    IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
    IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

    // チーム情報が両方取れた場合のみ比較する
    if (QueryTeamAgent && TargetTeamAgent)
    {
        return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
    }

    // TeamAgent を取得できない場合は安全側（敵対なし）として扱う
    return false;
}

/**
 * 攻撃者（InAttacker）と被弾者（InVictim）の位置関係から、
 * HitReact 用の方向 GameplayTag（Front/Back/Left/Right）を計算する。
 *
 * - Victim の Forward と「Victim→Attacker」方向ベクトルの内積から角度を求める
 * - 外積の Z を使って左右（符号）を決定する（Z<0 なら左側方向）
 * - OutAngleDifference には符号付き角度（度）を返す
 *
 * 返却する Tag は、被弾アニメやステート制御（GameplayTag）で使用する想定。
 *
 * @param InAttacker 攻撃者 Actor
 * @param InVictim 被弾者 Actor
 * @param OutAngleDifference Victim 正面基準の符号付き角度（度）
 * @return HitReact の方向を表す GameplayTag
 */
FGameplayTag UBluehorseFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference)
{
    check(InAttacker && InVictim);

    // 被弾者の前方向ベクトル
    const FVector VictimForward = InVictim->GetActorForwardVector();

    // 被弾者→攻撃者の方向（正規化）
    const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

    // 内積から角度（0～180）を算出
    const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
    OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

    // 外積の Z を使って左右を判定（Z<0 を左側扱いにして符号を付ける）
    const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);

    if (CrossResult.Z < 0.f)
    {
        OutAngleDifference *= -1.f;
    }

    // 角度範囲に応じて方向 Tag を返す
    if (OutAngleDifference >= -45.f && OutAngleDifference <= 45.f)
    {
        return BluehorseGameplayTags::Shared_Status_HitReact_Front;
    }
    else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
    {
        return BluehorseGameplayTags::Shared_Status_HitReact_Left;
    }
    else if (OutAngleDifference < -135.f || OutAngleDifference > 135.f)
    {
        return BluehorseGameplayTags::Shared_Status_HitReact_Back;
    }
    else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
    {
        return BluehorseGameplayTags::Shared_Status_HitReact_Right;
    }

    // 範囲外は念のため Front 扱い
    return BluehorseGameplayTags::Shared_Status_HitReact_Front;
}

bool UBluehorseFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
    // どちらかが無効な場合はデバッグ中にクラッシュ（開発時検出用）
    check(InAttacker && InDefender);

    // 防御者(Defender)の正面ベクトルと、攻撃者(Attacker)の正面ベクトルの内積を取得
    // DotProduct > 0 : 同じ方向を向いている（背後からの攻撃）
    // DotProduct < 0 : 反対方向を向いている（正面からの攻撃）
    const float DotResult = FVector::DotProduct(InDefender->GetActorForwardVector(), InAttacker->GetActorForwardVector());

    // 正面からの攻撃（DefenderがAttackerに向かっている）場合のみブロック可能とする
    return DotResult < 0.f;
}

/**
 * 入力方向（Input）を Actor の正面基準で 8 方向の回避方向（ERollDirection）に変換する。
 *
 * - ActorForward（2D）と InputNormal の内積から角度（0～180度）を算出
 * - 外積の Z により左右の符号を付与し、符号付き角度（-180～180度）にする
 * - 22.5度刻みで 8 方向に量子化して ERollDirection を返す
 *
 * @param InActor 回避方向の基準となる Actor（Forward を参照）
 * @param Input 入力ベクトル（移動入力など）
 * @param OutAngleDifference Actor 正面を 0 度とした符号付き角度（度）
 * @return 入力方向に対応する ERollDirection（8方向）
 *
 */
ERollDirection UBluehorseFunctionLibrary::ComputeRollDirection(AActor* InActor, const FVector& Input, float& OutAngleDifference)
{
    // Actor の正面方向（Yaw のみで判定したいので 2D 正規化）
    FVector ActorForward = InActor->GetActorForwardVector().GetSafeNormal2D();

    // 入力方向を正規化（移動入力を方向として扱う）
    FVector InputNormal = Input.GetSafeNormal();

    // 内積から角度（0～180度）を算出
    const float DotResult = FVector::DotProduct(ActorForward, InputNormal);
    OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

    // 外積の Z で左右を判定し、符号付き角度に変換（Z<0 を左側扱い）
    const FVector CrossResult = FVector::CrossProduct(ActorForward, InputNormal);

    if (CrossResult.Z < 0.f)
    {
        OutAngleDifference *= -1.f;
    }

    // 角度範囲を 22.5 度刻みで 8方向に分類
    if (OutAngleDifference >= -22.5f && OutAngleDifference < 22.5f) return ERollDirection::Forward;
    else if (OutAngleDifference >= 22.5f && OutAngleDifference < 67.5f) return ERollDirection::ForwardRight;
    else if (OutAngleDifference >= 67.5f && OutAngleDifference < 112.5f) return ERollDirection::Right;
    else if (OutAngleDifference >= 112.5f && OutAngleDifference < 157.5f) return ERollDirection::BackRight;
    else if (OutAngleDifference >= 157.5f || OutAngleDifference < -157.5f) return ERollDirection::Back;
    else if (OutAngleDifference >= -157.5f && OutAngleDifference < -112.5f) return ERollDirection::BackLeft;
    else if (OutAngleDifference >= -112.5f && OutAngleDifference < -67.5f) return ERollDirection::Left;
    else if (OutAngleDifference >= -67.5f && OutAngleDifference < -22.5f) return ERollDirection::ForwardLeft;

    // 想定外（計算誤差など）の場合は Forward にフォールバック
    Debug::Print(TEXT("NOT in range"), FColor::Red, 5);
    return ERollDirection::Forward;
}

/**
 * WorldContextObject から UBluehorseGameInstance を安全に取得するヘルパー関数。
 *
 * - Blueprint / C++ のどちらからでも呼び出せるよう WorldContextObject を受け取る
 * - GEngine → World → GameInstance の順で段階的に取得することで安全性を確保
 * - 取得に失敗した場合は nullptr を返す（呼び出し側での null チェック前提）
 *
 * @param WorldContextObject World を取得するためのコンテキストオブジェクト
 * @return UBluehorseGameInstance* 取得できた場合は有効なポインタ、失敗時は nullptr
 */
UBluehorseGameInstance* UBluehorseFunctionLibrary::GetBluehorseGameInstance(const UObject* WorldContextObject)
{
    // Engine が存在しない場合は取得不可（エディタ起動直後など）
    if (GEngine)
    {
        // WorldContextObject から World を取得
        // 失敗時はログを出力し nullptr を返す
        if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
        {
            // World に紐づく GameInstance を取得（Bluehorse 用にキャスト）
            return World->GetGameInstance<UBluehorseGameInstance>();
        }
    }

    // World / GameInstance が取得できなかった場合
    return nullptr;
}

/**
 * プレイヤーの入力モード（Game / UI / GameAndUI）を切り替えるヘルパー関数。
 *
 * - WorldContextObject から PlayerController を安全に取得
 * - WidgetToFocus が指定されている場合は UI フォーカスを設定
 *
 * @param WorldContextObject World を取得するためのコンテキスト
 * @param InInputMode 切り替える入力モード
 * @param WidgetToFocus UI 操作時にフォーカスさせたい Widget（任意）
 */
void UBluehorseFunctionLibrary::ToggleInputMode(const UObject* WorldContextObject, EBluehorseInputMode InInputMode, UUserWidget* WidgetToFocus)
{
    APlayerController* PlayerController = nullptr;

    // WorldContextObject から PlayerController を取得
    if (GEngine)
    {
        if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
        {
            PlayerController = World->GetFirstPlayerController();
        }
    }

    // PlayerController が取得できなければ何もしない
    if (!PlayerController)
    {
        return;
    }

    // 各入力モード用の設定オブジェクト
    FInputModeGameOnly GameOnlyMode;
    FInputModeUIOnly UIOnlyMode;
    FInputModeGameAndUI GameAndUIMode;

    switch (InInputMode)
    {
    case EBluehorseInputMode::GameOnly:
        // ゲーム入力のみ有効（UI 操作不可）
        PlayerController->SetInputMode(GameOnlyMode);
        PlayerController->bShowMouseCursor = false;

        break;

    case EBluehorseInputMode::UIOnly:
        // UI 操作のみ有効（ゲーム操作不可）
        if (WidgetToFocus)
        {
            UIOnlyMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
        }

        // マウスをビューポートにロックしない
        UIOnlyMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PlayerController->SetInputMode(UIOnlyMode);
        PlayerController->bShowMouseCursor = false;

        break;

    case EBluehorseInputMode::GameAndUI:
        // ゲーム操作と UI 操作の両方を有効化
        if (WidgetToFocus)
        {
            GameAndUIMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
        }
        UIOnlyMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PlayerController->SetInputMode(GameAndUIMode);
        PlayerController->bShowMouseCursor = false;

    default:
        break;
    }
}

/**
 * GameplayEffectSpecHandle を指定したターゲット Actor に適用するヘルパー関数。
 *
 * - Instigator / Target の両方から AbilitySystemComponent を取得
 * - 事前に生成済みの GameplayEffectSpecHandle を使用して効果を適用
 * - 適用結果を bool で返すため、ヒットリアクションや追加イベントの分岐に利用可能
 *
 * @param InInstigator 効果を発生させた Actor（攻撃者・発射者など）
 * @param InTargetActor 効果を受ける Actor（被弾者）
 * @param InSpecHandle 事前に作成済みの GameplayEffectSpecHandle
 * @return GameplayEffect の適用に成功した場合 true
 */
bool UBluehorseFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, FGameplayEffectSpecHandle& InSpecHandle)
{
    // 両アクターから AbilitySystemComponent を取得
    UBluehorseAbilitySystemComponent* SourceASC = NativeGetBluehorseASCFromActor(InInstigator);
    UBluehorseAbilitySystemComponent* TargetASC = NativeGetBluehorseASCFromActor(InTargetActor);

    // GameplayEffectSpec をターゲットに適用
    FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);

    // 適用が成功したかを返す
    return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

/**
 * ドーナツ状（内半径?外半径）の範囲内に、
 * ランダムな位置で Projectile をスポーンさせるための座標リストを生成する。
 *
 * - XY平面上に円環（Donut）状に配置
 * - 高さ方向にオフセットを加えることで地面埋まりを防止
 * - ランダム配置のため毎回異なるパターンになる
 *
 * @param Center スポーンの中心位置
 * @param NumProjectiles 生成する Projectile の数
 * @param InnerRadius ドーナツの内半径
 * @param OuterRadius ドーナツの外半径
 * @param HeightOffset 高さ方向のオフセット
 * @return 各 Projectile のスポーン位置配列
 */
TArray<FVector> UBluehorseFunctionLibrary::GetDonutSpawnPositions(const FVector& Center, int32 NumProjectiles, float InnerRadius, float OuterRadius, float HeightOffset)
{
    // 出力用配列を宣言
    TArray<FVector> SpawnPositions;

    // Projectile数分のメモリをあらかじめ確保（再確保を防ぎパフォーマンス向上）
    SpawnPositions.Reserve(NumProjectiles);

    // 高さ方向に補正を加えた基準位置（キャラクターの少し上など）
    const FVector Base = Center + FVector(0, 0, HeightOffset);

    // Projectileの個数分ループ
    for (int32 i = 0; i < NumProjectiles; ++i)
    {
        // 0?360度の範囲でランダムな角度を決定
        const float Angle = FMath::RandRange(0.f, 2 * PI);

        // 内半径?外半径の範囲でランダムな距離を決定
        const float Radius = FMath::RandRange(InnerRadius, OuterRadius);

        // 角度と距離からオフセット座標を算出（XY平面上のドーナツ座標）
        FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * Radius;

        // 基準位置（Base）にオフセットを加算して最終的な出現位置を算出
        SpawnPositions.Add(Base + Offset);
    }

    // 全Projectileの出現位置リストを返す
    return SpawnPositions;
}

/**
 * LevelSequence を「指定Transform（TransformOrigin）」で再生し、
 * シーケンサー側の Binding Tag を使って任意Actorに動的バインドするヘルパー。
 *
 * 目的：
 * - Level に LevelSequenceActor を常設せず、任意の場所で演出を再生できるようにする
 * - プレイヤー等の参照を「タグバインディング」で差し替え、汎用シーケンスを実現する
 *
 * 前提：
 * - SequenceAsset 側で、バインド対象のトラック（例：プレイヤー）に
 *   "PlayerActor" の Binding Tag が設定されていること
 *
 * 実装の要点：
 * - ULevelSequencePlayer::CreateLevelSequencePlayer で Player/Actor を生成
 * - ALevelSequenceActor::bOverrideInstanceData を true にし、InstanceData を有効化
 * - DefaultInstanceData（UDefaultLevelSequenceInstanceData）の TransformOrigin に SpawnTransform を設定
 *   → シーケンス再生座標を任意位置にオフセットできる
 * - AddBindingByTag でタグに紐づくバインドを TargetActorToBind に差し替える
 *
 * @param WorldContextObject World取得用コンテキスト（Widget/Ability等から呼べる）
 * @param SequenceAsset 再生する LevelSequence アセット
 * @param SpawnTransform 再生位置・向きの基準（TransformOrigin）
 * @param TargetActorToBind シーケンサーの "PlayerActor" バインドに差し替える Actor
 * @param Execs 成功/失敗（BP分岐用）
 */
void UBluehorseFunctionLibrary::PlayLevelSequenceAtTransform(const UObject* WorldContextObject, ULevelSequence* SequenceAsset, const FTransform& SpawnTransform, AActor* TargetActorToBind, EBluehorseSuccessType& Execs)
{
    // --- 実行コンテキストの検証 ----------------------------------------
    if (!WorldContextObject)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayLevelSequenceAtTransform] Invalid WorldContextObject"));
        Execs = EBluehorseSuccessType::Failed;
        return;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayLevelSequenceAtTransform] World not found"));
        Execs = EBluehorseSuccessType::Failed;
        return;
    }


    if (!SequenceAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayLevelSequenceAtTransform] SequenceAsset is null"));
        Execs = EBluehorseSuccessType::Failed;
        return;
    }

    // --- LevelSequencePlayer の生成 -------------------------------------
    // 自動再生はせず、初期化が完了してから Play する
    FMovieSceneSequencePlaybackSettings PlaybackSettings;
    PlaybackSettings.bAutoPlay = false;

    // CreateLevelSequencePlayer は ALevelSequenceActor を内部生成して返す
    ALevelSequenceActor* OutActor = nullptr;
    ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
        World,
        SequenceAsset,
        PlaybackSettings,
        OutActor
    );

    if (!SequencePlayer || !OutActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayLevelSequenceAtTransform] Failed to create LevelSequencePlayer"));
        Execs = EBluehorseSuccessType::Failed;
        return;
    }

    // --- TransformOrigin（再生座標系）を使うため InstanceData を有効化 -----
    OutActor->bOverrideInstanceData = true;

    // --- タグバインディングで対象 Actor を差し替え -----------------------
    // シーケンサー上で “PlayerActor” タグを持つActorを、実際のプレイヤーキャラに置き換える
    // これにより、カットシーン中も実際のキャラクターが参照される
    OutActor->AddBindingByTag(FName("PlayerActor"), TargetActorToBind, false);

    // --- TransformOrigin を設定して再生位置・向きを制御 -------------------
    // UDefaultLevelSequenceInstanceData に TransformOrigin を書き込むことで
    // シーケンスの座標系（原点）を SpawnTransform に変更できる
    if (UDefaultLevelSequenceInstanceData* DefaultLevelSequenceInstanceData = Cast<UDefaultLevelSequenceInstanceData>(OutActor->DefaultInstanceData))
    {
        DefaultLevelSequenceInstanceData->TransformOrigin = SpawnTransform;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayLevelSequenceAtTransform] Failed to cast"));
        Execs = EBluehorseSuccessType::Failed;
        return;
    }

    // --- シーケンス再生 --------------------------------------------------
    SequencePlayer->Play();

    Execs = EBluehorseSuccessType::Successful;
    return;
}
