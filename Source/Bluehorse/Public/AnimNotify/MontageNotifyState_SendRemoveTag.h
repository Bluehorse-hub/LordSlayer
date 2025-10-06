// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "GameplayTagContainer.h" 
#include "MontageNotifyState_SendRemoveTag.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UMontageNotifyState_SendRemoveTag : public UAnimNotify_PlayMontageNotifyWindow
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag EventTagOnBegin;

	// EndÇ≈äOÇ∑Loose Gameplay TagÅiîCà”Åj
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag TagToRemoveOnEnd;

	static bool IsGameWorld(const USkeletalMeshComponent* Mesh);
};

