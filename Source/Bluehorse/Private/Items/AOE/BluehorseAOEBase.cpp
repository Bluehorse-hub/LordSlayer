// Shungen All Rights Reserved.


#include "Items/AOE/BluehorseAOEBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BluehorseFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BluehorseGameplayTags.h"

/**
 * AOE（範囲効果）用 Actor の基底クラス。
 *
 * - 一定範囲内の Pawn を検索し、ダメージや効果を適用する
 * - 見た目（Niagara）とロジック（Overlap / Damage）を分離
 * - 実際の効果内容は Blueprint / 派生クラスで拡張可能
 */
ABluehorseAOEBase::ABluehorseAOEBase()
{
	// Tick は不要（生成時や明示的なタイミングで処理する設計）
	PrimaryActorTick.bCanEverTick = false;

	// AOE の見た目用 Niagara コンポーネント
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

	// 自動再生せず、必要なタイミングで制御する
	NiagaraComponent->bAutoActivate = false;

}

void ABluehorseAOEBase::BeginPlay()
{
	Super::BeginPlay();

	// 生成時に AOE 判定を実行
	SpawnAOESphere();
}

/**
 * AOE 範囲内に Actor が存在した際に呼ばれる処理。
 *
 * - BlueprintNativeEvent として定義されており、
 *   派生クラスや Blueprint 側で効果内容を自由に実装できる
 */
void ABluehorseAOEBase::OnAOEOverlapActor_Implementation(AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("Found overlapping actor: %s"), *OtherActor->GetName());
}

/**
 * AOE の球形判定を生成し、範囲内の Actor を検索する。
 *
 * - CollisionComponent は使わず、SphereOverlapActors による即時検索を行う
 * - 一度きりの判定や、タイミング制御がしやすい構成
 */
void ABluehorseAOEBase::SpawnAOESphere_Implementation()
{
	FVector SpherePosition = GetActorLocation();

	// デバッグ用に AOE 範囲を可視化
	if (bDrawDebugSphere)
	{
		UKismetSystemLibrary::DrawDebugSphere(
			GetWorld(),
			SpherePosition,
			SphereRadius,
			12,
			FLinearColor::Green,
			2.0f,
			1.0f
		);
	}

	// 検索対象のObjectType
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 無視するアクター
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetInstigator());

	// 結果を格納する配列
	TArray<AActor*> OutActors;

	// SphereOverlapActors呼び出し
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		SpherePosition,
		SphereRadius,
		ObjectTypes,
		nullptr,
		ActorsToIgnore,
		OutActors
	);

	// 結果確認
	if (bHit)
	{
		for (AActor* Actor : OutActors)
		{
			OnAOEOverlapActor(Actor);
		}
	}
}

/**
 * AOE ダメージを Pawn に適用する共通処理。
 *
 * - あらかじめ設定された GameplayEffectSpecHandle を使用する
 * - ダメージ適用後、HitReact 用の GameplayEvent を送信する
 *
 * @param InHitPawn ダメージ対象の Pawn
 * @param InPayload 付随情報（HitResult 等を含める想定）
 */
void ABluehorseAOEBase::HandleApplyAOEDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	// Projectile に設定されている GameplayEffectSpecHandle が有効かチェック
	checkf(AOEDamageEffectSpecHandle.IsValid(), TEXT("Forgot to assign a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());

	// Instigator（発射者）からヒットした Pawn へ GameplayEffect を適用
	const bool bWasApplied = UBluehorseFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, AOEDamageEffectSpecHandle);

	// 効果の適用が成功した場合のみ、ヒットリアクションイベントを発火
	if (bWasApplied)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			BluehorseGameplayTags::Shared_Event_HitReact,
			InPayload
		);
	}
}