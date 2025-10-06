// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "PawnHitEffectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPawnHitEffectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLUEHORSE_API IPawnHitEffectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UNiagaraSystem* GetHitEffect() const = 0;
	virtual USoundBase* GetHitSound() const { return nullptr; }
};
