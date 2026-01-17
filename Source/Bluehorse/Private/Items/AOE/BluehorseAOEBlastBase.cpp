// Shungen All Rights Reserved.


#include "Items/AOE/BluehorseAOEBlastBase.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"

/**
 * AOE 検索 + 爆発演出を実行する。
 *
 * - 親クラスで SphereOverlapActors により範囲内 Pawn を取得し、OnAOEOverlapActor を呼ぶ
 * - ここでは爆発の Niagara / サウンドを再生し、最後に自身を破棄する
 *
 * ※ BeginPlay 直後に 1回だけ判定して消える「即時爆発タイプ」の想定
 */
void ABluehorseAOEBlastBase::SpawnAOESphere_Implementation()
{
    // まずは AOE 範囲検索（Overlap した Actor に対して OnAOEOverlapActor が呼ばれる）
	Super::SpawnAOESphere_Implementation();

    FVector SpawnLocation = GetActorLocation();

    // --- VFX ---------------------------------------------------------------
    // 爆発の Niagara をその場に生成
	if (NiagaraEffect)
	{
        FRotator SpawnRotation = GetActorRotation();

        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            NiagaraEffect,
            SpawnLocation,
            SpawnRotation,
            FVector(1.0f),
            true,
            true,
            ENCPoolMethod::AutoRelease,
            true
        );
	}

    // --- SFX ---------------------------------------------------------------
    // 爆発音を再生（AutoDestroy = true で再生後に破棄）
    if (SpawnSound)
    {
        UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAtLocation(
            GetWorld(),
            SpawnSound,
            SpawnLocation,
            FRotator::ZeroRotator,
            1.0f, // Volume
            1.0f, // Pitch
            0.0f, // Start time
            nullptr, // Attenuation settings
            nullptr, // Concurrency settings
            true     // Auto destroy
        );
    }

    // 即時爆発タイプなので役目が終わったら自分を破棄
    Destroy();
}

/**
 * AOE 範囲内にいた Actor（主に Pawn）に対する処理。
 *
 * - Pawn のみを対象とし、敵対関係チェック、無敵/ブロック判定を行う
 * - ブロック成立時は「成功ブロック」イベントを送信
 * - ブロック不成立時は親クラス共通処理で GameplayEffect を適用し、HitReact を送信する
 */
void ABluehorseAOEBlastBase::OnAOEOverlapActor_Implementation(AActor* OtherActor)
{
    Super::OnAOEOverlapActor_Implementation(OtherActor);

    // --- 対象フィルタリング ------------------------------------------------
    // 衝突対象が Pawn でなければ無視
    APawn* HitPawn = Cast<APawn>(OtherActor);

    if (!HitPawn)
    {
        return;
    }

    // --- 敵対判定 ----------------------------------------------------------
    // 敵対関係がある場合のみダメージ適用
    if (!UBluehorseFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
    {
        return;
    }

    // --- 無敵判定 ----------------------------------------------------------
    // 無敵状態のキャラクターはダメージ無効
    const bool bIsPlayerInvincible = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitPawn, BluehorseGameplayTags::Player_Status_Invincible);

    // すり抜け動作になるように、そのまま終了
    if (bIsPlayerInvincible)
    {
        return;
    }

    bool bIsValidBlock = false;

    const bool bIsPlayerBlocking = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitPawn, BluehorseGameplayTags::Player_Status_Blocking);

    // --- ブロック判定 ------------------------------------------------------
    if (bIsPlayerBlocking)
    {
        // Projectileの角度や位置などから「有効なブロック」かどうかを判定
        bIsValidBlock = UBluehorseFunctionLibrary::IsValidBlock(this, HitPawn);
    }

    // --- GameplayEvent 用データ -------------------------------------------
    // GameplayEvent用のデータを作成（Instigator: Projectile, Target: 被弾者）
    FGameplayEventData Data;
    Data.Instigator = this;
    Data.Target = HitPawn;

    // --- 結果適用 ----------------------------------------------------------
    // Projectileに紐づくGameplayEffectSpecHandleを使ってダメージを適用
    if (bIsValidBlock)
    {
        // ブロックが成立した場合、GASイベントを発火
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            HitPawn,
            BluehorseGameplayTags::Player_Event_SuccessfulBlock,
            Data
        );
    }
    else
    {
        // ブロック不成立：GameplayEffect を適用し、HitReact を送信（親の共通処理）
        HandleApplyAOEDamage(HitPawn, Data);
    }
}
