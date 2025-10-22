// Shungen All Rights Reserved.


#include "AbilitySystem/AbilityTasks/AbilityTask_TurnToTarget.h"
#include "BluehorseFunctionLibrary.h"

//  毎フレームTickが必要なタスクであるため、bTickingTask = trueに設定。
UAbilityTask_TurnToTarget::UAbilityTask_TurnToTarget()
{
	bTickingTask = true;
}

UAbilityTask_TurnToTarget* UAbilityTask_TurnToTarget::TurnToTarget(UGameplayAbility* OwningAbility, AActor* TargetActor, float InterpSpeed, FGameplayTag EndTag)
{
	UAbilityTask_TurnToTarget* Node = NewAbilityTask<UAbilityTask_TurnToTarget>(OwningAbility);

	// パラメータ設定
	Node->Target = TargetActor;
	Node->RotationSpeed = InterpSpeed;
	Node->EndCheckTag = EndTag;
	return Node;
}

// 毎フレームターゲット方向を計算し、指定速度でアバターを回転させる。
// 特定のEndTagがアバターに付与された場合は自動的に終了。
void UAbilityTask_TurnToTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	// Broadcastできない or ターゲットが無効なら終了
	if (!ShouldBroadcastAbilityTaskDelegates() || !Target.IsValid())
	{
		EndTask();
		return;
	}

	// Abilityの所有アクター（実際に回転させるキャラ）を取得
	AActor* Avatar = GetAvatarActor();
	if (!Avatar) return;

	// 指定されたEndTagが付与されていればタスク終了
	if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(Avatar, EndCheckTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("[TurnToTarget] EndTag detected ? Task Ending."));
		EndTask();
		return;
	}

	// ターゲット方向への回転を計算（水平面のみ）
	FVector ToTarget = Target->GetActorLocation() - Avatar->GetActorLocation();
	ToTarget.Z = 0.f;

	if (ToTarget.IsNearlyZero()) return;

	// 現在角度と目標角度を取得
	FRotator CurrentRot = Avatar->GetActorRotation();
	FRotator TargetRot = ToTarget.Rotation();

	// RInterpToでスムーズに補間
	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotationSpeed);

	Avatar->SetActorRotation(NewRot);
}

// タスク終了処理
// 終了時にターゲット参照をクリアして安全に破棄。
void UAbilityTask_TurnToTarget::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	Target.Reset();  // WeakPtrのクリア
}


