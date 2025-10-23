// Shungen All Rights Reserved.


#include "Items/BluehorseProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "BluehorseDebugHelper.h"

ABluehorseProjectileBase::ABluehorseProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionProfileName(TEXT("Projectile"));
	//ProjectileCollisionBox->SetCollisionObjectType(ECC_GameTraceChannel1);
	//ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	//ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	//ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	//ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	//ProjectileCollisionBox->SetNotifyRigidBodyCollision(false);
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);
	ProjectileCollisionBox->ComponentTags.Add(FName(TEXT("IgnoreProjectile")));

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());
	ProjectileNiagaraComponent->ComponentTags.Add(FName(TEXT("IgnoreProjectile")));

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->ComponentTags.Add(FName(TEXT("IgnoreProjectile")));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 4.f;
}

void ABluehorseProjectileBase::SetTargetActorFromBBKey(APawn* SourcePawn, FName TargetKeyName)
{
	if (!ProjectileMovementComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetActorFromBBKey: ProjectileMovementComp is null"));
		return;
	}

	if (!IsValid(SourcePawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetActorFromBBKey: SourcePawn is invalid"));
		return;
	}

	// --- Controller取得（AI or Player どちらでも可） ---
	AAIController* AIController = Cast<AAIController>(SourcePawn->GetController());
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetActorFromBBKey: Controller is not AI (%s)"), *SourcePawn->GetName());
		return;
	}

	// --- Blackboard取得 ---
	UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	if (!BBComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetActorFromBBKey: BlackboardComponent is null (%s)"), *SourcePawn->GetName());
		return;
	}

	// --- 指定キーでターゲットを取得 ---
	UObject* TargetObj = BBComp->GetValueAsObject(TargetKeyName);
	AActor* TargetActor = Cast<AActor>(TargetObj);

	if (!IsValid(TargetActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetActorFromBBKey: Blackboard Key '%s' is invalid"), *TargetKeyName.ToString());
		return;
	}

	// --- ホーミング設定 ---
	ProjectileMovementComp->HomingTargetComponent = TargetActor->GetRootComponent();

	FString DebugMessage = FString::Printf(
		TEXT("SetTargetActorFromBBKey: Homing target set to %s via key '%s' (SourcePawn=%s)"),
		*TargetActor->GetName(),
		*TargetKeyName.ToString(),
		*SourcePawn->GetName()
	);

	//Debug::Print(DebugMessage, FColor::Green);
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

void ABluehorseProjectileBase::Destroyed()
{
	Super::Destroyed();

	// 自分で破棄した（命中など）なら何もしない
	if (bWasManuallyDestroyed)
	{
		return;
	}

	// 寿命で消滅した場合のみ実行
	BP_OnSpawnProjectileExpireFX();

	//Debug::Print(TEXT("Projectile expired naturally after lifespan"), FColor::Green);
}

void ABluehorseProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Projectileが反応してほしくないコンポーネントを無視
	// これにより、インタラクト用SphereなどのHitを除外できる
	if (OtherComp && OtherComp->ComponentHasTag(TEXT("IgnoreProjectile")))
	{
		UE_LOG(LogTemp, Verbose, TEXT("Ignored projectile overlap with %s (tagged IgnoreProjectile)"), *OtherComp->GetName());

		// --- 停止防止処理 ---
		if (ProjectileMovementComp)
		{
			// ホーミング弾ならターゲットを維持したまま速度再適用
			if (ProjectileMovementComp->bIsHomingProjectile && ProjectileMovementComp->HomingTargetComponent.IsValid())
			{
				const FVector ToTarget = (ProjectileMovementComp->HomingTargetComponent->GetComponentLocation() - GetActorLocation()).GetSafeNormal();
				ProjectileMovementComp->Velocity = ToTarget * ProjectileMovementComp->InitialSpeed;
			}
			else
			{
				// 直進弾なら進行方向を維持して速度再適用
				const FVector ForwardDir = GetActorForwardVector();
				ProjectileMovementComp->Velocity = ForwardDir * ProjectileMovementComp->InitialSpeed;
			}
		}

		return;
	}
	// 衝突対象がPawnかどうかを確認
	APawn* HitPawn = Cast<APawn>(OtherActor);

	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

	// 敵対関係にない場合はダメージ処理を行わず終了（味方や無関係オブジェクトへの誤ヒット防止）
	if (!HitPawn || !UBluehorseFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		HandleManualDestroy();
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
	HandleManualDestroy();
}

void ABluehorseProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap: Self=%s (Tags=%s) | Other=%s | OtherComp=%s (Tags=%s)"),
		*GetName(),
		*FString::JoinBy(ProjectileCollisionBox->ComponentTags, TEXT(","), [](const FName& Tag) { return Tag.ToString(); }),
		*OtherActor->GetName(),
		*OtherComp->GetName(),
		*FString::JoinBy(OtherComp->ComponentTags, TEXT(","), [](const FName& Tag) { return Tag.ToString(); })
	);

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
			// 回避成功時は追尾しないようにする
			if (ProjectileMovementComp)
			{
				ProjectileMovementComp->bIsHomingProjectile = false;
				ProjectileMovementComp->HomingTargetComponent = nullptr;

				// 速度を保ったまま直進させる
				const FVector CurrentDirection = ProjectileMovementComp->Velocity.GetSafeNormal();
				ProjectileMovementComp->Velocity = CurrentDirection * ProjectileMovementComp->InitialSpeed;
			}
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
			BP_OnSpawnProjectileHitFX(SweepResult.ImpactPoint);

			bool bIsValidBlock = false;

			const bool bIsPlayerBlocking = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitPawn, BluehorseGameplayTags::Player_Status_Blocking);


			if (bIsPlayerBlocking)
			{
				// Projectileの角度や位置などから「有効なブロック」かどうかを判定
				bIsValidBlock = UBluehorseFunctionLibrary::IsValidBlock(this, HitPawn);
			}

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
				HandleApplyProjectileDamage(HitPawn, Data);
			}

			// 命中後にProjectileを破棄
			// シングルヒット型のProjectileとして破壊
			HandleManualDestroy();
		}
	}
}

void ABluehorseProjectileBase::HandleManualDestroy()
{
	// 自然消滅ではなくHitなどで手動で破壊したかどうか
	bWasManuallyDestroyed = true;
	Destroy();
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

