// Shungen All Rights Reserved.


#include "Items/AOE/BluehorseAOEBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BluehorseFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BluehorseGameplayTags.h"

ABluehorseAOEBase::ABluehorseAOEBase()
{
	PrimaryActorTick.bCanEverTick = false;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->bAutoActivate = false;

}

void ABluehorseAOEBase::BeginPlay()
{
	Super::BeginPlay();

	SpawnAOESphere();
}

void ABluehorseAOEBase::OnAOEOverlapActor_Implementation(AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("Found overlapping actor: %s"), *OtherActor->GetName());
}

void ABluehorseAOEBase::SpawnAOESphere_Implementation()
{
	FVector SpherePosition = GetActorLocation();

	if (bDrawDebugSphere)
	{
		UKismetSystemLibrary::DrawDebugSphere(
			GetWorld(),
			SpherePosition,
			SphereRadius,
			12,
			FLinearColor::Green,
			2.0f,
			1.0f
		);
	}

	// 検索対象のObjectType
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 無視するアクター
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetInstigator());

	// 結果を格納する配列
	TArray<AActor*> OutActors;

	// SphereOverlapActors呼び出し
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		SpherePosition,
		SphereRadius,
		ObjectTypes,
		nullptr,
		ActorsToIgnore,
		OutActors
	);

	// 結果確認
	if (bHit)
	{
		for (AActor* Actor : OutActors)
		{
			OnAOEOverlapActor(Actor);
		}
	}
}

void ABluehorseAOEBase::HandleApplyAOEDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	// Projectile に設定されている GameplayEffectSpecHandle が有効かチェック
	checkf(AOEDamageEffectSpecHandle.IsValid(), TEXT("Forgot to assign a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());

	// Instigator（発射者）からヒットした Pawn へ GameplayEffect を適用
	const bool bWasApplied = UBluehorseFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, AOEDamageEffectSpecHandle);

	// 効果の適用が成功した場合のみ、ヒットリアクションイベントを発火
	if (bWasApplied)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			BluehorseGameplayTags::Shared_Event_HitReact,
			InPayload
		);

		UE_LOG(LogTemp, Warning, TEXT("AOE Damage applied to %s"), *InHitPawn->GetActorNameOrLabel());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleApplyAOEDamage: bWasApplied is False"));
	}
}