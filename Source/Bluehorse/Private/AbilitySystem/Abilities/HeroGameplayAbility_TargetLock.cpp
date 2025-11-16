// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/BluehorseWidgetBase.h"
#include "Controllers/BluehorseHeroController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"

#include "BluehorseDebugHelper.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	GetHeroCharacterFromActorInfo()->bUseControllerRotationYaw = true;

	TryLockOnTarget();
	InitTargetLockMappingContext();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetHeroCharacterFromActorInfo()->bUseControllerRotationYaw = false;

	ResetTargetLockMappingContext();
	// ターゲットロック終了時にはターゲットロックに関するものを破棄する
	CleanUp();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ロックオン処理中に毎フレーム呼ばれるTick関数。
// ロック対象の生存確認・回転制御・UI位置更新などを行う
void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	// ロック対象がいない、もしくはロック対象・自身が死亡したときはアビリティを終了し即座に終了
	if (!CurrentLockedActor ||
		UBluehorseFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor, BluehorseGameplayTags::Shared_Status_Dead) ||
		UBluehorseFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), BluehorseGameplayTags::Shared_Status_Dead)
		)
	{
		CancelTargetLockAbility();
		return;
	}

	// ロックオンUI（ターゲットアイコン）の位置を更新
	SetTargetLockWidgetPosition();

	// 回避（ローリング）中はロックオン方向の上書きを行わない
	//  ローリング中に強制回転すると操作感が悪くなるため
	const bool bShouldOverrideRotation = !UBluehorseFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), BluehorseGameplayTags::Player_Status_Rolling);

	// ロックオン中にキャラとカメラの向きを敵方向へ補間する
	if (bShouldOverrideRotation)
	{
		// プレイヤーからターゲットへのLookAt角度を計算
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			GetHeroCharacterFromActorInfo()->GetActorLocation(),
			CurrentLockedActor->GetActorLocation()
		);

		// カメラ視点補正（俯瞰など任意の微調整）
		LookAtRot -= FRotator(TargetLockCameraOffsetDistance, 0.f, 0.f);

		// Pitch・Roll を固定（キャラの制御はYaw中心のため）
		LookAtRot.Pitch = 0.f;
		LookAtRot.Roll = 0.f;

		// 現在のControlRotationとLookAtRotを補間することで
		// 一気に向きを変えず、自然にカメラがターゲットへ向く
		const FRotator CurrentControlRot = GetHeroControllerFromActorInfo()->GetControlRotation();
		const FRotator TargetRot = FMath::RInterpTo(CurrentControlRot, LookAtRot, DeltaTime, TargetLockRotationInterSpeed);

		// ControllerRotation を更新してカメラをターゲット方向に向ける。
		// ※ ActorRotationは書き換えないことで、キャラの小刻みな揺れの発生を防止。
		GetHeroControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));

		// ※ ActorRotationを直接いじるとアプリ化したものでキャラが小刻みな揺れたので、以下のような処理は今後しない
		// GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f, TargetRot.Yaw, 0.f));
	}
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	// Lock可能な対象を探す
	GetAvailableActorsToLock();

	// 対象の位置によって格納
	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock = nullptr;

	// Lock可能な対象を左右に振り分ける
	GetAvailableActorsAroundTarget(ActorsOnLeft, ActorsOnRight);

	// Lock対象を切り替えた際に左右で最も近い敵にLock対象を再設定する
	if (InSwitchDirectionTag == BluehorseGameplayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnLeft);
	}
	else
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnRight);
	}

	// 再設定が行われていればその対象をLock対象とする
	if (NewTargetToLock)
	{
		CurrentLockedActor = NewTargetToLock;
	}
}

// ターゲットロック開始処理
void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	// プレイヤーの周囲からロック可能なアクターを収集する
	GetAvailableActorsToLock();

	// ターゲットロック対象がいない場合は即座に終了する
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	// ロック可能なアクターの中から「最も近い」ターゲットを選ぶ
	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock);

	// 対象が見つかった場合：ロックオン開始
	if (CurrentLockedActor)
	{
		// まだウィジェットが生成されていなければ作成→Viewport に表示
		DrawTargetLockWidget();

		// UIを現在の対象位置に合わせる（最初のフレームで正しい位置に表示）
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{

	TArray<FHitResult> BoxTraceHits;

	//--- 公式ドキュメント ---//
	// Sweeps a box along the given line and returns all hits encountered.
	// This only finds objects that are of a type specified by ObjectTypes.
	// True if there was a hit, false otherwise.
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(), // 対象を探す際のStart地点
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetHeroCharacterFromActorInfo()->GetActorForwardVector() * BoxTraceDistance, //　対象を探す当たり判定が移動するの奥行
		TraceBoxSize / 2.f, // 当たり判定の大きさ
		GetHeroCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(), // 当たり判定が進む向き
		BoxTraceChannel, // 適用対象の列挙(Pawnなど)
		false, // 複雑衝突を行うか(false推奨)
		TArray<AActor*>(), // 判定から除外するActorの配列
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None, // デバッグ表示の種類
		BoxTraceHits, // ヒットの結果(複数)が入る
		true // 自身を自動で無視するか
	);

	// UKismetSystemLibrary::BoxTraceMultiForObjects()の処理の結果からヒットしたActorを調べる
	// HitActorが自身でなければ、ターゲットロック可能として記録する
	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			// デバッグ用の条件　!AvailableActorsToLock.Contains(HitActor)は不要
			if (HitActor != GetHeroCharacterFromActorInfo() && !AvailableActorsToLock.Contains(HitActor))
			{
				AvailableActorsToLock.AddUnique(HitActor);
				// Debug::Print(HitActor->GetActorNameOrLabel());
			}
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = 0.f;

	// 公式ドキュメント
	// Returns an Actor nearest to Origin from ActorsToCheck array.
	// ターゲットロック可能な対象から自身に最も近いものを求める。今回は距離は必要無いので適当な値で埋める
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(), InAvailableActors, ClosestDistance);
}

// 対象を切り替える際に対象が左右どちらにいるか確認する処理
void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight)
{
	// Lock対象がいない場合は終了する
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	// プレイヤー位置
	const FVector PlayerLocation = GetHeroCharacterFromActorInfo()->GetActorLocation();

	// プレイヤーから現在ロック中ターゲットの方向ベクトル（正規化）
	const FVector PlayerToCurrentNormalized = (CurrentLockedActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

	// Lock可能な対象について左右どちらにいるか確認する
	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		// Lock可能でない、対象を現在ロック中なら処理しない
		if (!AvailableActor || AvailableActor == CurrentLockedActor) continue;

		// プレイヤーから対象の方向（正規化）
		const FVector PlayerToAvailableNormalized = (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

		// 外積を使用して左右判定を行う
		// CrossResult.Z の符号で左右を判断できる
		const FVector CrossResult = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);

		if (CrossResult.Z > 0.f)
		{
			// 右側に位置するアクター
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			// 左側に位置するアクター
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	// まだUIが生成されていない場合のみ生成処理を行う
	if (!DrawnTargetLockWidget)
	{
		// TargetLockWidgetClassが未設定の場合はエラーを出す
		checkf(TargetLockWidgetClass, TEXT("Forgot to assign a valid widget class in BP"));

		// ウィジェット生成
		DrawnTargetLockWidget = CreateWidget<UBluehorseWidgetBase>(GetHeroControllerFromActorInfo(), TargetLockWidgetClass);

		// ウィジェット生成に失敗した場合はcheckで実行を停止
		check(DrawnTargetLockWidget);

		// ウィジェットをViewportに追加して画面に表示
		DrawnTargetLockWidget->AddToViewport();
	}
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}

	FVector2D ScreenPosition;
	// 公式ドキュメント
	// Gets the projected world to screen position for a player,
	// then converts it into a widget position, which takes into account any quality scaling.
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetHeroControllerFromActorInfo(), // どのプレイヤー視点で座標を求めるか
		CurrentLockedActor->GetActorLocation(), // 表示するワールド空間上の座標
		ScreenPosition, // 出力用のFVector2D変数
		true // ビューポート内での相対位置を返すように設定
	);

	// Widgetの大きさを考慮して描画しないと位置がずれてしまう
	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		// Widget内の要素を走査して、SizeBoxの幅と高さにアクセスし値を保持する
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
			[this](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			}
		);
	}

	// SizeBoxの大きさから描画位置を調整する
	ScreenPosition -= (TargetLockWidgetSize / 2.f);
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition, false);

}

// ターゲットロック用のInputMappingContextをEnhancedInputに登録する処理
// このアビリティが有効化されたタイミングで呼ばれ、
// プレイヤーがロックオン関連の入力を利用できるようにする
void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	// プレイヤーコントローラーを取得し、
	// そのLocalPlayerからEnhancedInputのSubsystemを取り出す
	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer();

	// LocalPlayer に紐づく EnhancedInputLocalPlayerSubsystem を取得
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	// Subsystemがnullの場合は致命的エラーとして停止する
	check(Subsystem);

	// ターゲットロック用のInputMappingContextを登録
	Subsystem->AddMappingContext(TargetLockMappingContext, 3);
}

// ターゲットロックアビリティを外部要因（死亡・対象消失など）で中断させる処理
// GASのCancelAbility()を安全に呼び出すためのラッパ関数
void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

// ターゲットロックアビリティ終了時に呼ばれるクリーンアップ処理
void UHeroGameplayAbility_TargetLock::CleanUp()
{
	// ロック候補リストをクリア。
	// アビリティごとに再収集されるため、終了時に必ず空にしておく
	AvailableActorsToLock.Empty();

	// 現在ロック中のターゲット参照を消す
	CurrentLockedActor = nullptr;

	// ターゲットロックUI（画面上のターゲットマーカー）を非表示にする
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}

	// Widget のポインタをリセット（次回ロックオン開始時に再生成される）
	DrawnTargetLockWidget = nullptr;

	// ターゲットロックUIのサイズキャッシュもリセット
	TargetLockWidgetSize = FVector2D::ZeroVector;

	// キャラ移動速度など、ロックオン専用にキャッシュしていたステータスを初期化
	CachedDefaultMaxwalkSpeed = 0.f;
}

// ターゲットロックアビリティ終了時に、
// Enhanced Input に追加した「ロックオン専用の操作設定」を解除する処理
void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	// プレイヤーコントローラーが存在しない場合は何もしない
	if (!GetHeroControllerFromActorInfo())
	{
		return;
	}

	// EnhancedInputはLocalPlayer単位で管理されるため、
	// PlayerControllerではなくLocalPlayerを取得する必要がある
	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer();

	// LocalPlayerに紐づくEnhancedInputLocalPlayerSubsystemを取得
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	// Subsystemが存在しない場合は実行を停止
	check(Subsystem);

	// ロックオン用のInputMappingContextを解除する
	// ※ AddMappingContext と完全に対になる処理。
	//    これを呼ばないと、ロックオンが終わった後も
	//    ロックオン関連の入力（切替・解除など）が有効なままになり、
	//    意図しない操作が起きる可能性がある
	Subsystem->RemoveMappingContext(TargetLockMappingContext);
}
