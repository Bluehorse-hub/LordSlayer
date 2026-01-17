// Shungen All Rights Reserved.


#include "AnimNotify/MontageNotifyState_SendRemoveTag.h"
#include "BluehorseFunctionLibrary.h"

#include "BluehorseDebugHelper.h"

// MontageNotifyState によって、指定 GameplayTag を
//「区間開始で付与」→「区間終了で除去」する。

void UMontageNotifyState_SendRemoveTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// エディタプレビュー等（非ゲームワールド）では何もしない
	if (!IsGameWorld(MeshComp)) return;

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	if (!Owner)
	{
		// 取得失敗は異常系（Owner不在）なのでデバッグ表示
		Debug::Print(TEXT("when add, can not get owner"), FColor::Red, 1);
		return;
	}

	// 区間開始：タグ付与（重複付与しないユーティリティを使用）
	UBluehorseFunctionLibrary::AddGameplayTagToActorIfNone(Owner, EventTagOnBegin);
}

void UMontageNotifyState_SendRemoveTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!IsGameWorld(MeshComp)) return;

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	if (!Owner)
	{
		Debug::Print(TEXT("when remove, can not get owner"), FColor::Red, 2);
		return;
	}

	// 区間終了：指定タグが付いている場合のみ除去する
	if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(Owner, TagToRemoveOnEnd))
	{
		// 重要：
		// NotifyEnd のタイミングで即 Remove すると、
		// 「同フレーム内のアニメ更新／GASイベント／モンタージュ終了処理」と順序競合して
		// 不安定になることがあるため、NextTick に遅延して安全に除去する。
		//
		// - Owner が破棄されていても安全なように Weak 参照で保持
		// - 次フレーム頭で Remove することでフレーム内競合を回避
		TWeakObjectPtr<AActor> WeakOwner = Owner;
		const FGameplayTag Tag = TagToRemoveOnEnd;

		MeshComp->GetWorld()->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateLambda([WeakOwner, Tag]()
				{
					if (AActor* O = WeakOwner.Get())
					{
						UBluehorseFunctionLibrary::RemoveGameplayTagFromActorIfFound(O, Tag);
					}
				}
			)
		);
	}
}

// Notify が「実ゲーム中」に呼ばれているかの判定
// エディターでのクラッシュ防止用関数
bool UMontageNotifyState_SendRemoveTag::IsGameWorld(const USkeletalMeshComponent* Mesh)
{
	const UWorld* World = Mesh ? Mesh->GetWorld() : nullptr;
	return World && World->IsGameWorld();
}

