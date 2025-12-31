// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interact/BluehorseBaseInteract.h"
#include "BluehorseDoorInteract.generated.h"

/**
 * 
 */

class ALevelSequenceActor;
class ULevelSequencePlayer;
class ULevelSequence;

UCLASS()
class BLUEHORSE_API ABluehorseDoorInteract : public ABluehorseBaseInteract
{
	GENERATED_BODY()

public:
	ABluehorseDoorInteract();
	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Door")
	void RequestToggle();

protected:
	UFUNCTION(BlueprintCallable)
	void PlayOpenDoorSequence();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LeftDoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* RightDoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sequence")
	ULevelSequence* OpenSequence;

	UPROPERTY()
	ALevelSequenceActor* DoorSequenceActor;

	UPROPERTY()
	ULevelSequencePlayer* DoorSequencePlayer;


private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsOpen = false;
};
