// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BluehorseTypes/BluehorseEnumTypes.h"
#include "ToggleWeaponCollision.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UToggleWeaponCollision : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	static bool IsGameWorld(const USkeletalMeshComponent* Mesh);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EToggleDamageType ToggleDamageType;
};
