// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "GameplayEffectTypes.h"
#include "BluehorseAOEBase.generated.h"

struct FGameplayEventData;

UCLASS()
class BLUEHORSE_API ABluehorseAOEBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABluehorseAOEBase();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnAOEOverlapActor(AActor* OtherActor);
	virtual void OnAOEOverlapActor_Implementation(AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent)
	void SpawnAOESphere();
	virtual void SpawnAOESphere_Implementation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SphereRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDrawDebugSphere = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AOE")
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AOE")
	UNiagaraSystem* NiagaraEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE")
	USoundBase* SpawnSound;

	UPROPERTY(BlueprintReadOnly, Category = "AOE", meta = (ExposeOnSpawn = "true"))
	FGameplayEffectSpecHandle AOEDamageEffectSpecHandle;

	void HandleApplyAOEDamage(APawn* InHitPawn, const FGameplayEventData& InPayload);
};
