// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/HitResult.h"
#include "BluehorseWeaponBase.generated.h"

class UBoxComponent;
class USceneComponent;

DECLARE_DELEGATE_TwoParams(FOnTargetInteractedTwoParamsDelegate, AActor*, const FHitResult&)
DECLARE_DELEGATE_OneParam(FOnTargetInteractedOneParamDelegate, AActor*)

UCLASS()
class BLUEHORSE_API ABluehorseWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABluehorseWeaponBase();

	FOnTargetInteractedTwoParamsDelegate OnWeaponHitTarget;
	FOnTargetInteractedOneParamDelegate OnWeaponPulledFromTarget;

	TArray<AActor*> IgnoreActors;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	UBoxComponent* WeaponCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapons")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapons")
	bool bShowBoxDebug = false;

	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void BoxTrace(FHitResult& BoxHit);

public:
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const { return WeaponCollisionBox; }
};
