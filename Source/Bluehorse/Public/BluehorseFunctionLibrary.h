// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BluehorseTypes/BluehorseEnumTypes.h"
#include "BluehorseFunctionLibrary.generated.h"

class UBluehorseAbilitySystemComponent;
class UPawnCombatComponent;
class UBluehorseGameInstance;


/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UBluehorseAbilitySystemComponent* NativeGetBluehorseASCFromActor(AActor* InActor);
	
	UFUNCTION(BlueprintCallable, Category = "Bluehorse|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EBluehorseConfirmType& OutConfirmType);

	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|FunctionLibrary", meta = (DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, EBluehorseValidType& OutValidType);

	UFUNCTION(BlueprintPure, Category = "Bluehorse|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);

	UFUNCTION(BlueprintPure, Category = "Bluehorse|FunctionLibrary")
	static FGameplayTag ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference);

	UFUNCTION(BlueprintPure, Category = "Bluehorse|FunctionLibrary")
	static bool IsValidBlock(AActor* InAttacker, AActor* InDefender);

	UFUNCTION(BlueprintPure, Category = "Bluehorse|FunctionLibrary")
	static ERollDirection ComputeRollDirection(AActor* InActor, const FVector& Input, float& OutAngleDifference);

	UFUNCTION(BlueprintPure, Category = "Bluehorse|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static UBluehorseGameInstance* GetBluehorseGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static void ToggleInputMode(const UObject* WorldContextObject, EBluehorseInputMode InInputMode, UUserWidget* WidgetToFocus = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|FunctionLibrary")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, FGameplayEffectSpecHandle& InSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|FunctionLibrary")
	static TArray<FVector> GetDonutSpawnPositions(const FVector& Center, int32 NumProjectiles, float InnerRadius, float OuterRadius, float HeightOffset);
};
