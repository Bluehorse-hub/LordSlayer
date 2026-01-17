// Shungen All Rights Reserved.


#include "AnimNotify/ToggleWeaponCollision.h"
#include "BluehorseFunctionLibrary.h"
#include "Components/Combat/PawnCombatComponent.h"

#include "BluehorseDebugHelper.h"

// AnimNotifyState によって、武器の当たり判定（Collision）を
// 「区間開始で有効化」→「区間終了で無効化」するクラス
void UToggleWeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// エディタプレビューなど、非ゲームワールドでは処理しない
	if (!IsGameWorld(MeshComp)) return;

	// このアニメを再生している Actor
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	// Owner から CombatComponent を取得
	UPawnCombatComponent* CombatComponent = UBluehorseFunctionLibrary::NativeGetPawnCombatComponentFromActor(Owner);

	// 区間開始：武器の当たり判定を有効化
	// ToggleDamageType によって、攻撃種別（軽/重/属性など）を切り替える想定
	CombatComponent->ToggleWeaponCollision(true, ToggleDamageType);
}

void UToggleWeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!IsGameWorld(MeshComp)) return;

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	UPawnCombatComponent* CombatComponent = UBluehorseFunctionLibrary::NativeGetPawnCombatComponentFromActor(Owner);

	// 区間終了：武器の当たり判定を無効化
	// 付けっぱなし事故を防ぐため、Begin/End を必ず対にしている
	CombatComponent->ToggleWeaponCollision(false, ToggleDamageType);
}

// Notify が実ゲーム中に呼ばれているかの判定
bool UToggleWeaponCollision::IsGameWorld(const USkeletalMeshComponent* Mesh)
{
	const UWorld* World = Mesh ? Mesh->GetWorld() : nullptr;
	return World && World->IsGameWorld();
}
