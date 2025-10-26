// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/Attacks/GA_HeroAttackMaster.h"
#include "BluehorseFunctionLibrary.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "BluehorseTypes/BluehorseStructTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BluehorseGameplayTags.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Ability終了時のタグ除去処理
void UGA_HeroAttackMaster::RemoveTagsOnEndAbility()
{
    AActor* Avatar = GetAvatarActorFromActorInfo();

    if (Avatar)
    {
        // Ability終了時に削除すべきタグ一覧を走査
        for (const FGameplayTag& Tag : TagsToRemoveOnEnd)
        {
            // アクターが指定タグを持っている場合のみ削除
            if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(Avatar, Tag))
            {
                UBluehorseFunctionLibrary::RemoveGameplayTagFromActorIfFound(Avatar, Tag);
            }
        }
    }
}

// 右手武器のダメージSpecHandleを生成
FGameplayEffectSpecHandle UGA_HeroAttackMaster::GetHeroEquippedRightHandWeaponDamageSpecHandle()
{
    int WeaponBaseDamage = GetHeroCombatComponentFromActorInfo()->GetHeroEquippedRightHandWeaponDamageAtLevel(GetAbilityLevel());

    return MakeHeroDamageEffectSpecHandle(EffectClass, WeaponBaseDamage, CurrentAttackTypeTag, GetMotionValueFromDataTable());
}

// 左手武器のダメージSpecHandleを生成
FGameplayEffectSpecHandle UGA_HeroAttackMaster::GetHeroEquippedLeftHandWeaponDamageSpecHandle()
{
    int WeaponBaseDamage = GetHeroCombatComponentFromActorInfo()->GetHeroEquippedLeftHandWeaponDamageAtLevel(GetAbilityLevel());

    return MakeHeroDamageEffectSpecHandle(EffectClass, WeaponBaseDamage, CurrentAttackTypeTag, GetMotionValueFromDataTable());
}

// ダメージを実際に適用する処理
void UGA_HeroAttackMaster::HandleApplyDamage(const FGameplayEventData& EventData)
{
    // 対象アクターを取得（TargetはTObjectPtr<const AActor>なのでconst_castを使用）
    AActor* TargetActor = const_cast<AActor*>(EventData.Target.Get());

    if (!TargetActor)
    {
        return;
    }

    FActiveGameplayEffectHandle ActiveGameplayEffectHandle;

    // 使用している武器（右手／左手）に応じてSpecHandleを取得し、Effectを適用
    if (IsRightWeapon)
    {
        ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, GetHeroEquippedRightHandWeaponDamageSpecHandle());
    }
    else
    {
        ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, GetHeroEquippedLeftHandWeaponDamageSpecHandle());
    }

    // 成功時にヒットリアクションイベントを発火（GASイベント通知）
    if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
    {
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            TargetActor,
            BluehorseGameplayTags::Shared_Ability_HitReact,
            EventData
        );
    }
}

// モーション値（攻撃倍率など）をDataTableから取得
float UGA_HeroAttackMaster::GetMotionValueFromDataTable()
{
    const FHeroMotionDataRow* Row = MotionValueDataTable->FindRow<FHeroMotionDataRow>(
        AttackName,
        TEXT("ContextString")
    );

    if (!Row)
    {
        return 1.0f;
    }

    return Row->MotionValue;
}

// 攻撃方向を入力ベクトルから算出してMotionWarpingに反映
void UGA_HeroAttackMaster::ComputeAttackDirection()
{
    ABluehorseHeroCharacter* Hero = GetHeroCharacterFromActorInfo();
    if (!Hero)
    {
        return;
    }

    FVector2D LastInput2D(0.f, 0.f);

    // 最後の移動入力を取得（入力がない場合はゼロベクトル）
    if (Hero->GetCharacterMovement())
    {
        FVector InputVec = Hero->GetLastMovementInputVector();
        LastInput2D = FVector2D(InputVec.X, InputVec.Y);
    }

    // 入力方向を正規化（2D平面上）
    UKismetMathLibrary::Normalize2D(LastInput2D);
    FVector CachedAttackDirection = FVector(LastInput2D.X, LastInput2D.Y, 0.f);

    // 入力がほぼゼロの場合はキャラの前方方向を使用
    if (CachedAttackDirection.IsNearlyZero(0.0001f))
    {
        CachedAttackDirection = Hero->GetActorForwardVector();
    }

    // MotionWarpingコンポーネントを取得
    UMotionWarpingComponent* MotionWarpingComp = Hero->GetMotionWarpingComponent();
    if (!MotionWarpingComp)
    {
        return;
    }

    // 攻撃方向に基づく回転を生成
    const FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(CachedAttackDirection);

    // MotionWarpingのWarpターゲットを更新
    MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(NormalAttackWarpTargetName, FVector::ZeroVector, TargetRotation);
}

bool UGA_HeroAttackMaster::CanPerformFatalAttackByTag(AActor*& OutHitActor, float LineTraceDistance)
{
    // Abilityの実行者（＝プレイヤー）を取得
    AActor* Executer = GetAvatarActorFromActorInfo();
    if (!Executer)
    {
        return false;
    }

    UWorld* World = Executer->GetWorld();
    if (!World)
    {
        return false;
    }

    FVector ExecuterLocation = Executer->GetActorLocation();
    FVector ExecuterForward = Executer->GetActorForwardVector();
    FVector TraceEnd = ExecuterLocation + ExecuterForward * LineTraceDistance;

    // Trace設定
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Executer);

    FHitResult HitResult;

    const bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
        World,
        ExecuterLocation,
        TraceEnd,
        ObjectTypes,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        HitResult,
        true
    );

    if (bHit)
    {
        if (AActor* HitActor = HitResult.GetActor())
        {
            // 致命一撃可能タグを持つか確認
            if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitActor, BluehorseGameplayTags::Shared_Status_Knockdown))
            {
                OutHitActor = HitActor;
                return true;
            }
        }
    }

    return false;
}

void UGA_HeroAttackMaster::TryTriggerFatalAttackIfPossible(EBluehorseSuccessType& Result)
{
    Result = EBluehorseSuccessType::Failed;
    UE_LOG(LogTemp, Log, TEXT("[Fatal] TryTriggerFatalAttackIfPossible called."));


    ABluehorseHeroCharacter* Hero = GetHeroCharacterFromActorInfo();
    if (!Hero)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Fatal] Hero reference invalid."));
        return;
    }

    // MotionWarpingコンポーネントを取得
    UMotionWarpingComponent* MotionWarpingComp = Hero->GetMotionWarpingComponent();
    if (!MotionWarpingComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Fatal] MotionWarpingComponent not found."));
        return;
    }

    AActor* OutHitActor = nullptr;
    if (CanPerformFatalAttackByTag(OutHitActor, 200.f))
    {
        UE_LOG(LogTemp, Log, TEXT("[Fatal] CanPerformFatalAttackByTag returned TRUE. Target: %s"),
            *GetNameSafe(OutHitActor));

        // GameplayEvent用のデータを作成
        FGameplayEventData EventData;
        EventData.Instigator = Hero;
        EventData.Target = OutHitActor;
        EventData.EventTag = BluehorseGameplayTags::Shared_Event_Executed;

        // GASイベント送信（BlueprintでのSendGameplayEventToActor相当）
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            OutHitActor,
            BluehorseGameplayTags::Shared_Event_Executed,
            EventData
        );

        UE_LOG(LogTemp, Log, TEXT("[Fatal] GameplayEvent sent to target: %s"), *GetNameSafe(OutHitActor));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[Fatal] Fatal check failed (no target or not valid)."));
        return;
    }

    const FVector OutHitActorLocation = OutHitActor->GetActorLocation();
    const FVector OutHitActorFwdVector = OutHitActor->GetActorForwardVector();

    FVector TargetLocation = OutHitActorLocation + OutHitActorFwdVector * FVector(100.f, 100.f, 0.f);

    const FVector ReversedVector = OutHitActorFwdVector * -1.0f;
    const FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(ReversedVector);

    UE_LOG(LogTemp, Log, TEXT("[Fatal] Warp TargetLocation: %s, TargetRotation: %s"),
        *TargetLocation.ToString(), *TargetRotation.ToString());

    bool bSetActorRotation = Hero->SetActorRotation(TargetRotation);

    if (bSetActorRotation)
    {
        UE_LOG(LogTemp, Log, TEXT("[Fatal] Hero rotation successfully set to face target."));
        UE_LOG(LogTemp, Log, TEXT("[Fatal] MotionWarp Target Updated (Name: %s)"),
            *FatalAttackWarpTargetName.ToString());

        MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(FatalAttackWarpTargetName, TargetLocation, TargetRotation);
        Result = EBluehorseSuccessType::Successful;
        UE_LOG(LogTemp, Log, TEXT("[Fatal] TryTriggerFatalAttackIfPossible completed SUCCESSFULLY."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Fatal] Hero rotation could not be set."));
        return;
    }
}
