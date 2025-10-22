// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h" 
#include "AnimNotifyState_SendEventTag.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UAnimNotifyState_SendEventTag : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag EventTagOnBegin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag EventTagOnEnd;

};
