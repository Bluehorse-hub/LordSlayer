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

// 敵が攻撃したときに敵同士がダメージを与えあわないようにするための判定を行う
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
    // どちらかが無効な場合はデバッグ中にクラッシュ（開発時検出用）
    check(InAttacker && InDefender);

    // 防御者(Defender)の正面ベクトルと、攻撃者(Attacker)の正面ベクトルの内積を取得
    // DotProduct > 0 : 同じ方向を向いている（背後からの攻撃）
    // DotProduct < 0 : 反対方向を向いている（正面からの攻撃）
    const float DotResult = FVector::DotProduct(InDefender->GetActorForwardVector(), InAttacker->GetActorForwardVector());

    // 正面からの攻撃（DefenderがAttackerに向かっている）場合のみブロック可能とする
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
    // 両アクターから AbilitySystemComponent を取得
    UBluehorseAbilitySystemComponent* SourceASC = NativeGetBluehorseASCFromActor(InInstigator);
    UBluehorseAbilitySystemComponent* TargetASC = NativeGetBluehorseASCFromActor(InTargetActor);

    // GameplayEffectSpec をターゲットに適用
    FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);

    // 適用が成功したかを返す
    return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

AActor* UBluehorseFunctionLibrary::GetTargetActor(const UObject* WorldContext, FName TargetKeyName)
{
    // WorldContext（呼び出し元のProjectileやAbilityなど）から所有者のAIを特定
    if (!WorldContext) return nullptr;

    UWorld* World = WorldContext->GetWorld();
    if (!World) return nullptr;

    // --- プレイヤー or 敵AIキャラを取得（今回はAI前提） ---
    ACharacter* OwnerCharacter = Cast<ACharacter>(WorldContext->GetOuter());

    if (!OwnerCharacter) return nullptr;

    // --- AIControllerを取得 ---
    AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController());
    if (!AIController) return nullptr;

    // --- BlackboardComponentを取得 ---
    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp) return nullptr;

    // --- Blackboardの "TargetActor" キーから値を取得 ---
    UObject* TargetObj = BlackboardComp->GetValueAsObject(TargetKeyName);

    return Cast<AActor>(TargetObj);
}

