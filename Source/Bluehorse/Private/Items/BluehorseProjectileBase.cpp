// Shungen All Rights Reserved.


#include "Items/BluehorseProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "BluehorseDebugHelper.h"

ABluehorseProjectileBase::ABluehorseProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 4.f;
}

void ABluehorseProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// 飛び道具が貫通するかどうかなどに使う予定
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

void ABluehorseProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 衝突対象がPawnかどうかを確認
	APawn* HitPawn = Cast<APawn>(OtherActor);

	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

	// 敵対関係にない場合はダメージ処理を行わず終了（味方や無関係オブジェクトへの誤ヒット防止）
	if (!HitPawn || !UBluehorseFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		Destroy();
		return;
	}

	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitPawn, BluehorseGameplayTags::Player_Status_Blocking);


	if (bIsPlayerBlocking)
	{
		// Projectileの角度や位置などから「有効なブロック」かどうかを判定
		bIsValidBlock = UBluehorseFunctionLibrary::IsValidBlock(this, HitPawn);
	}

	// GameplayEvent用のデータを作成（Instigator: Projectile, Target: 被弾者）
	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;

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
		HandleApplyProjectileDamage(HitPawn, Data);
	}

	// ヒット後のProjectileを破棄
	Destroy();
}

void ABluehorseProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Projectileが反応してほしくないコンポーネントを無視
	// これにより、インタラクト用SphereなどのOverlapを除外できる
	if (OtherComp && OtherComp->ComponentHasTag(TEXT("IgnoreProjectile")))
	{
		UE_LOG(LogTemp, Verbose, TEXT("Ignored projectile overlap with %s (tagged IgnoreProjectile)"), *OtherComp->GetName());
		return;
	}

	// 同じアクターに対しては1回だけ処理する
	if (OverlappedActors.Contains(OtherActor))
	{
		return;
	}

	// 処理済みリストに追加
	OverlappedActors.AddUnique(OtherActor);

	// 相手がPawnでない場合は無視
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		// 無敵状態のキャラクターはダメージ無効
		const bool bIsPlayerInvincible = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitPawn, BluehorseGameplayTags::Player_Status_Invincible);

		// すり抜け動作になるように、そのまま終了
		if (bIsPlayerInvincible)
		{
			return;
		}

		// GameplayEventDataを構築
		// ダメージやイベントを送る際に使用するデータ構造体
		// Instigator（発射者）とTarget（被弾者）を設定
		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;

		// 敵対関係の判定
		// 敵対関係がある場合のみダメージ適用
		if (UBluehorseFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			// Projectileに紐づくGameplayEffectSpecHandleを使ってダメージを適用
			HandleApplyProjectileDamage(HitPawn, Data);

			// 命中後にProjectileを破棄
			// シングルヒット型のProjectileとして破壊
			Destroy();
		}
	}
}

void ABluehorseProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	// Projectile に設定されている GameplayEffectSpecHandle が有効かチェック
	checkf(ProjectileDamageEffectSpecHandle.IsValid(), TEXT("Forgot to assign a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());

	// Instigator（発射者）からヒットした Pawn へ GameplayEffect を適用
	const bool bWasApplied = UBluehorseFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);

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

