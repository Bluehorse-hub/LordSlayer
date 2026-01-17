// Shungen All Rights Reserved.


#include "Items/Weapons/BluehorseWeaponBase.h"
#include "Components/BoxComponent.h"
#include "BluehorseFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "BluehorseDebugHelper.h"

/**
 * 武器Actorの基底クラス。
 *
 * - 見た目（WeaponMesh）と当たり判定（WeaponCollisionBox）を持つ
 * - Overlap は「当たった可能性」を拾うトリガーとして利用し、
 *   実際のヒット確定は BoxTrace で行う（二重判定で精度を上げる）
 * - ヒット結果はデリゲートで外部（CombatComponent等）へ通知する
 */
ABluehorseWeaponBase::ABluehorseWeaponBase()
{
 	// Tick は不要（当たり判定は Overlap/Trace のイベント駆動）
	PrimaryActorTick.bCanEverTick = false;

	// --- Mesh --------------------------------------------------------------
	// 武器の見た目（Root）
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	// --- Collision ---------------------------------------------------------
	// 武器の当たり判定用 Box
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// --- Trace Points ------------------------------------------------------
	// BoxTrace の開始/終了位置（武器の動きに合わせたスイープ判定に利用）
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStart"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEnd"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());

	// 当たり判定用
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
}

/**
 * CollisionBox が他Actorと重なった瞬間。
 *
 * - 友軍/敵対チェックを行い、敵対対象にのみ処理する
 * - その後 BoxTrace を行い、確定ヒット位置/Actor を取得してデリゲート通知する
 */
void ABluehorseWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 武器の所有者を取得する
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	// Pawn 以外は基本的に対象外（必要ならここを拡張）
	APawn* HitPawn = Cast<APawn>(OtherActor);

	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instigator as the owning pawn for the weapon: %s"), *GetName());

	// 敵対していない対象には当たらない（Friendly Fire防止）
	if (!UBluehorseFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
	{
		return;
	}

	// Overlap は「当たった可能性」のみなので、BoxTrace で確定判定を取る
	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if (BoxHit.GetActor())
	{
		APawn* BoxHitPawn = Cast<APawn>(BoxHit.GetActor());

		// Trace の確定結果も敵対チェック（環境物などを除外）
		if (!UBluehorseFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, BoxHitPawn))
		{
			return;
		}

		//「ヒット確定」を通知
		OnWeaponHitTarget.ExecuteIfBound(BoxHit.GetActor(), BoxHit);
	}
}

/**
 * CollisionBox の重なりが解けた瞬間。
 *
 * - 敵対対象から武器が抜けたタイミングとして扱い、
 *   ヒットストップ解除等の処理を外部へ通知できる
 */
void ABluehorseWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 武器の所有者を取得する
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instigator as the owning pawn for the weapon: %s"), *GetName());

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		// 敵対対象なら「武器が抜けた」通知を送る
		if (UBluehorseFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
		{
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
		}

		// TODO: Implemnt hit check for enemy character
	}
}

/**
 * BoxTrace によるヒット確定判定。
 *
 * - BoxTraceStart -> BoxTraceEnd の間を Box 形状でスイープして最初のヒットを取得する
 * - IgnoreActors によって同一攻撃中の多段ヒット（同じ相手への連続ヒット）を抑制する
 */
void ABluehorseWeaponBase::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	// Trace で無視する Actor
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxTraceExtent,
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true
	);

	// ヒットした Actor を記録し、同一攻撃中の重複ヒットを防ぐ
	IgnoreActors.AddUnique(BoxHit.GetActor());
}