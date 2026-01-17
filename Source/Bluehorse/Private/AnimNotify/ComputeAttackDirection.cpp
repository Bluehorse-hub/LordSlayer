// Shungen All Rights Reserved.


#include "AnimNotify/ComputeAttackDirection.h"
#include "Characters/BluehorseHeroCharacter.h"

// 攻撃アニメ区間中に、プレイヤーの入力方向へ向きを補間して合わせる AnimNotifyState

void UComputeAttackDirection::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// エディタプレビュー等（非ゲームワールド）では動かさない
	if (!IsGameWorld(MeshComp)) return;

	// 毎Tickの Cast を避けるため、Begin で HeroCharacter をキャッシュしておく
	CachedHeroCharacter = Cast<ABluehorseHeroCharacter>(MeshComp->GetOwner());
}

// 毎フレーム呼ばれる
void UComputeAttackDirection::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!IsGameWorld(MeshComp)) return;

	TryStartTurn();
	UpdateTurn(FrameDeltaTime);
}

// アニメ区間終了時に呼ばれる
void UComputeAttackDirection::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!IsGameWorld(MeshComp)) return;

	bIsTurning = false;
	CachedHeroCharacter.Reset();
}

// Notify が「実ゲーム中」に呼ばれているかの判定
bool UComputeAttackDirection::IsGameWorld(const USkeletalMeshComponent* Mesh)
{
	const UWorld* World = Mesh ? Mesh->GetWorld() : nullptr;
	return World && World->IsGameWorld();
}

// ターン開始判定
void UComputeAttackDirection::TryStartTurn()
{
	if (bIsTurning) return;

	FVector InputVector = CachedHeroCharacter->GetLastMovementInputVector();
	if (InputVector.IsNearlyZero()) return;

	FRotator CurrentRot = CachedHeroCharacter->GetActorRotation();
	FRotator InputRot = InputVector.Rotation();

	// 現在Yawと入力方向Yawの差分（最短回転角）を求める
	float AngleDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, InputRot.Yaw));

	bIsTurning = true;
	TurnInterpAlpha = 0.f;

	InitialYaw = CurrentRot.Yaw;

	// 目標は「最短角度の差分Yaw」：InitialYaw + TargetYaw が最終到達Yawになる
	TargetYaw = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, InputRot.Yaw);
}

// ターン更新
void UComputeAttackDirection::UpdateTurn(float DeltaTime)
{
	if (!bIsTurning) return;

	// Turnにかかっている時間を更新していく
	TurnInterpAlpha += DeltaTime / TurnDuration;

	// Turnの推移は線形で行う
	float InterpYaw = FMath::Lerp(InitialYaw, InitialYaw + TargetYaw, TurnInterpAlpha);

	FRotator NewRot = CachedHeroCharacter->GetActorRotation();
	NewRot.Yaw = InterpYaw;

	CachedHeroCharacter->SetActorRotation(NewRot);

	// FMath::Lerp()のAlphaは０～１の値をとる
	if (TurnInterpAlpha >= 1.f)
	{
		bIsTurning = false;
	}
}
