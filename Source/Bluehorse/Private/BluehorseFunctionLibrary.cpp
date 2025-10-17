// Shungen All Rights Reserved.


#include "BluehorseFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BluehorseGameplayTags.h"
#include "BluehorseGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "BluehorseDebugHelper.h"

UBluehorseAbilitySystemComponent* UBluehorseFunctionLibrary::NativeGetBluehorseASCFromActor(AActor* InActor)
{
    if (!IsValid(InActor)) return nullptr;

    return Cast<UBluehorseAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UBluehorseFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
    if (!TagToAdd.IsValid()) return;

    UBluehorseAbilitySystemComponent* ASC = NativeGetBluehorseASCFromActor(InActor);

    if (!ASC->HasMatchingGameplayTag(TagToAdd))
    {
        ASC->AddLooseGameplayTag(TagToAdd);
    }
}

void UBluehorseFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
    if (!TagToRemove.IsValid()) return;

    UBluehorseAbilitySystemComponent* ASC = NativeGetBluehorseASCFromActor(InActor);

    if (ASC->HasMatchingGameplayTag(TagToRemove))
    {
        ASC->RemoveLooseGameplayTag(TagToRemove);
    }
}

bool UBluehorseFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
    if (!IsValid(InActor)) return false;

    UBluehorseAbilitySystemComponent* ASC = NativeGetBluehorseASCFromActor(InActor);

    if (!IsValid(ASC)) return false;

    return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UBluehorseFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EBluehorseConfirmType& OutConfirmType)
{
    OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EBluehorseConfirmType::Yes : EBluehorseConfirmType::No;
}

UPawnCombatComponent* UBluehorseFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
    if (!IsValid(InActor))
    {
        return nullptr;
    }


    if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
    {
        return PawnCombatInterface->GetPawnCombatComponent();
    }

    return nullptr;
}

UPawnCombatComponent* UBluehorseFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor, EBluehorseValidType& OutValidType)
{
    UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

    OutValidType = CombatComponent ? EBluehorseValidType::Valid : EBluehorseValidType::Invalid;
    return CombatComponent;
}

// �G���U�������Ƃ��ɓG���m���_���[�W��^������Ȃ��悤�ɂ��邽�߂̔�����s��
bool UBluehorseFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{

    if (!QueryPawn || !TargetPawn)
    {
        return false;
    }

    check(QueryPawn && TargetPawn);

    IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
    IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

    if (QueryTeamAgent && TargetTeamAgent)
    {
        return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
    }

    return false;
}

FGameplayTag UBluehorseFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference)
{
    check(InAttacker && InVictim);

    const FVector VictimForward = InVictim->GetActorForwardVector();
    const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

    const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
    OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

    const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);

    if (CrossResult.Z < 0.f)
    {
        OutAngleDifference *= -1.f;
    }

    if (OutAngleDifference >= -45.f && OutAngleDifference <= 45.f)
    {
        return BluehorseGameplayTags::Shared_Status_HitReact_Front;
    }
    else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
    {
        return BluehorseGameplayTags::Shared_Status_HitReact_Left;
    }
    else if (OutAngleDifference < -135.f || OutAngleDifference > 135.f)
    {
        return BluehorseGameplayTags::Shared_Status_HitReact_Back;
    }
    else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
    {
        return BluehorseGameplayTags::Shared_Status_HitReact_Right;
    }

    return BluehorseGameplayTags::Shared_Status_HitReact_Front;
}

bool UBluehorseFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
    // �ǂ��炩�������ȏꍇ�̓f�o�b�O���ɃN���b�V���i�J�������o�p�j
    check(InAttacker && InDefender);

    // �h���(Defender)�̐��ʃx�N�g���ƁA�U����(Attacker)�̐��ʃx�N�g���̓��ς��擾
    // DotProduct > 0 : ���������������Ă���i�w�ォ��̍U���j
    // DotProduct < 0 : ���Ε����������Ă���i���ʂ���̍U���j
    const float DotResult = FVector::DotProduct(InDefender->GetActorForwardVector(), InAttacker->GetActorForwardVector());

    // ���ʂ���̍U���iDefender��Attacker�Ɍ������Ă���j�ꍇ�̂݃u���b�N�\�Ƃ���
    return DotResult < 0.f;
}

ERollDirection UBluehorseFunctionLibrary::ComputeRollDirection(AActor* InActor, const FVector& Input, float& OutAngleDifference)
{
    FVector ActorForward = InActor->GetActorForwardVector().GetSafeNormal2D();
    FVector InputNormal = Input.GetSafeNormal();

    const float DotResult = FVector::DotProduct(ActorForward, InputNormal);
    OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

    const FVector CrossResult = FVector::CrossProduct(ActorForward, InputNormal);

    if (CrossResult.Z < 0.f)
    {
        OutAngleDifference *= -1.f;
    }

    if (OutAngleDifference >= -22.5f && OutAngleDifference < 22.5f) return ERollDirection::Forward;
    else if (OutAngleDifference >= 22.5f && OutAngleDifference < 67.5f) return ERollDirection::ForwardRight;
    else if (OutAngleDifference >= 67.5f && OutAngleDifference < 112.5f) return ERollDirection::Right;
    else if (OutAngleDifference >= 112.5f && OutAngleDifference < 157.5f) return ERollDirection::BackRight;
    else if (OutAngleDifference >= 157.5f || OutAngleDifference < -157.5f) return ERollDirection::Back;
    else if (OutAngleDifference >= -157.5f && OutAngleDifference < -112.5f) return ERollDirection::BackLeft;
    else if (OutAngleDifference >= -112.5f && OutAngleDifference < -67.5f) return ERollDirection::Left;
    else if (OutAngleDifference >= -67.5f && OutAngleDifference < -22.5f) return ERollDirection::ForwardLeft;

    Debug::Print(TEXT("NOT in range"), FColor::Red, 5);
    return ERollDirection::Forward;
}

bool UBluehorseFunctionLibrary::CanFatalAttack(AActor* InExecuter, FGameplayTag TagToCheck, AActor*& OutHitActor, float Distance)
{
    if (!InExecuter) return false;

    UWorld* World = InExecuter->GetWorld();

    if (!World) return false;

    FVector ExecuterLocation = InExecuter->GetActorLocation();
    FVector ExecuterForward = InExecuter->GetActorForwardVector();
    FVector TraceEnd = ExecuterLocation + ExecuterForward * Distance;

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(InExecuter);

    FHitResult HitResult;

    bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
        World,
        ExecuterLocation,
        TraceEnd,
        ObjectTypes,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        HitResult,
        true
    );

    if (bHit)
    {
        if (AActor* HitActor = HitResult.GetActor())
        {
            if (NativeDoesActorHaveTag(HitActor, TagToCheck))
            {
                OutHitActor = HitActor;
                return true;
            }
        }
    }

    return false;
}

UBluehorseGameInstance* UBluehorseFunctionLibrary::GetBluehorseGameInstance(const UObject* WorldContextObject)
{
    
    if (GEngine)
    {
        if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
        {
            return World->GetGameInstance<UBluehorseGameInstance>();
        }
    }

    return nullptr;
}

void UBluehorseFunctionLibrary::ToggleInputMode(const UObject* WorldContextObject, EBluehorseInputMode InInputMode, UUserWidget* WidgetToFocus)
{
    APlayerController* PlayerController = nullptr;

    if (GEngine)
    {
        if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
        {
            PlayerController = World->GetFirstPlayerController();
        }
    }

    if (!PlayerController)
    {
        return;
    }

    FInputModeGameOnly GameOnlyMode;
    FInputModeUIOnly UIOnlyMode;
    FInputModeGameAndUI GameAndUIMode;

    switch (InInputMode)
    {
    case EBluehorseInputMode::GameOnly:

        PlayerController->SetInputMode(GameOnlyMode);
        PlayerController->bShowMouseCursor = false;

        break;

    case EBluehorseInputMode::UIOnly:

        if (WidgetToFocus)
        {
            UIOnlyMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
        }
        UIOnlyMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PlayerController->SetInputMode(UIOnlyMode);
        PlayerController->bShowMouseCursor = true;

        break;

    case EBluehorseInputMode::GameAndUI:
        if (WidgetToFocus)
        {
            GameAndUIMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
        }
        UIOnlyMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PlayerController->SetInputMode(GameAndUIMode);
        PlayerController->bShowMouseCursor = false;

    default:
        break;
    }
}

bool UBluehorseFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, FGameplayEffectSpecHandle& InSpecHandle)
{
    // ���A�N�^�[���� AbilitySystemComponent ���擾
    UBluehorseAbilitySystemComponent* SourceASC = NativeGetBluehorseASCFromActor(InInstigator);
    UBluehorseAbilitySystemComponent* TargetASC = NativeGetBluehorseASCFromActor(InTargetActor);

    // GameplayEffectSpec ���^�[�Q�b�g�ɓK�p
    FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);

    // �K�p��������������Ԃ�
    return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

TArray<FVector> UBluehorseFunctionLibrary::GetDonutSpawnPositions(const FVector& Center, int32 NumProjectiles, float InnerRadius, float OuterRadius, float HeightOffset)
{
    // �o�͗p�z���錾
    TArray<FVector> SpawnPositions;

    // Projectile�����̃����������炩���ߊm�ہi�Ċm�ۂ�h���p�t�H�[�}���X����j
    SpawnPositions.Reserve(NumProjectiles);

    // ���������ɕ␳����������ʒu�i�L�����N�^�[�̏�����Ȃǁj
    const FVector Base = Center + FVector(0, 0, HeightOffset);

    // Projectile�̌������[�v
    for (int32 i = 0; i < NumProjectiles; ++i)
    {
        // 0?360�x�͈̔͂Ń����_���Ȋp�x������
        const float Angle = FMath::RandRange(0.f, 2 * PI);

        // �����a?�O���a�͈̔͂Ń����_���ȋ���������
        const float Radius = FMath::RandRange(InnerRadius, OuterRadius);

        // �p�x�Ƌ�������I�t�Z�b�g���W���Z�o�iXY���ʏ�̃h�[�i�c���W�j
        FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * Radius;

        // ��ʒu�iBase�j�ɃI�t�Z�b�g�����Z���čŏI�I�ȏo���ʒu���Z�o
        SpawnPositions.Add(Base + Offset);
    }

    // �SProjectile�̏o���ʒu���X�g��Ԃ�
    return SpawnPositions;
}
