// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "ComputeAttackDirection.generated.h"

class ABluehorseHeroCharacter;
/**
 * 
 */
UCLASS()
class BLUEHORSE_API UComputeAttackDirection : public UAnimNotify_PlayMontageNotifyWindow
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	static bool IsGameWorld(const USkeletalMeshComponent* Mesh);

private:
	void TryStartTurn();
	void UpdateTurn(float DeltaTime);

	TWeakObjectPtr<ABluehorseHeroCharacter> CachedHeroCharacter;
	bool bIsTurning = false;

	UPROPERTY(EditDefaultsOnly, Category = "Turn")
	float TurnDuration = 0.15f;
	float TurnInterpAlpha = 0.f;
	float TurnInterpSpeed = 1.f;
	float InitialYaw = 0.f;
	float TargetYaw = 0.f;
	
};
