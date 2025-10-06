// Shungen All Rights Reserved.


#include "AnimNotify/MontageNotifyState_SendRemoveTag.h"
#include "BluehorseFunctionLibrary.h"

#include "BluehorseDebugHelper.h"

void UMontageNotifyState_SendRemoveTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!IsGameWorld(MeshComp)) return;

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	if (!Owner)
	{
		Debug::Print(TEXT("when add, can not get owner"), FColor::Red, 1);
		return;
	}

	//UE_LOG(LogTemp, Log, TEXT("NotifyBegin SendEvent:%s"), *EventTagOnBegin.ToString());
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

	if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(Owner, TagToRemoveOnEnd))
	{
		//UE_LOG(LogTemp, Log, TEXT("NotifyEnd   RemoveTag:%s"), *TagToRemoveOnEnd.ToString());
		// UBluehorseFunctionLibrary::RemoveGameplayTagFromActorIfFound(Owner, TagToRemoveOnEnd);
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

// エディターでのクラッシュ防止用関数
bool UMontageNotifyState_SendRemoveTag::IsGameWorld(const USkeletalMeshComponent* Mesh)
{
	const UWorld* World = Mesh ? Mesh->GetWorld() : nullptr;
	return World && World->IsGameWorld();
}

