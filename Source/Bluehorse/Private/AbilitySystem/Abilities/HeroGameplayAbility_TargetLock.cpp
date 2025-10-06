// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/BluehorseWidgetBase.h"
#include "Controllers/BluehorseHeroController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"

#include "BluehorseDebugHelper.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	InitTargetLockMappingContext();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetTargetLockMappingContext();
	// �^�[�Q�b�g���b�N�I�����ɂ̓^�[�Q�b�g���b�N�Ɋւ�����̂�j������
	CleanUp();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ���b�N�Ώۂ�ǐՂ��邽�߂�Tick�������s��
void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	// ���b�N�Ώۂ����Ȃ��A�������̓��b�N�ΏہE���g�����S�����Ƃ��̓A�r���e�B���I���������ɏI��
	if (!CurrentLockedActor ||
		UBluehorseFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor, BluehorseGameplayTags::Shared_Status_Dead) ||
		UBluehorseFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), BluehorseGameplayTags::Shared_Status_Dead)
		)
	{
		CancelTargetLockAbility();
		return;
	}

	SetTargetLockWidgetPosition();

	const bool bShouldOverrideRotation = !UBluehorseFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), BluehorseGameplayTags::Player_Status_Rolling);

	if (bShouldOverrideRotation)
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			GetHeroCharacterFromActorInfo()->GetActorLocation(),
			CurrentLockedActor->GetActorLocation()
		);

		LookAtRot -= FRotator(TargetLockCameraOffsetDistance, 0.f, 0.f);

		const FRotator CurrentControlRot = GetHeroControllerFromActorInfo()->GetControlRotation();
		const FRotator TargetRot = FMath::RInterpTo(CurrentControlRot, LookAtRot, DeltaTime, TargetLockRotationInterSpeed);

		GetHeroControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f, TargetRot.Yaw, 0.f));
	}
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	// Lock�\�ȑΏۂ�T��
	GetAvailableActorsToLock();

	// �Ώۂ̈ʒu�ɂ���Ċi�[
	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock = nullptr;

	// Lock�\�ȑΏۂ����E�ɐU�蕪����
	GetAvailableActorsAroundTarget(ActorsOnLeft, ActorsOnRight);

	// Lock�Ώۂ�؂�ւ����ۂɍ��E�ōł��߂��G��Lock�Ώۂ��Đݒ肷��
	if (InSwitchDirectionTag == BluehorseGameplayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnLeft);
	}
	else
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnRight);
	}

	// �Đݒ肪�s���Ă���΂��̑Ώۂ�Lock�ΏۂƂ���
	if (NewTargetToLock)
	{
		CurrentLockedActor = NewTargetToLock;
	}
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();

	// �^�[�Q�b�g���b�N�Ώۂ����Ȃ��ꍇ�͑����ɏI������
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock);

	if (CurrentLockedActor)
	{
		DrawTargetLockWidget();

		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{

	TArray<FHitResult> BoxTraceHits;

	//--- �����h�L�������g ---//
	// Sweeps a box along the given line and returns all hits encountered.
	// This only finds objects that are of a type specified by ObjectTypes.
	// True if there was a hit, false otherwise.
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(), // �Ώۂ�T���ۂ�Start�n�_
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetHeroCharacterFromActorInfo()->GetActorForwardVector() * BoxTraceDistance, //�@�Ώۂ�T�������蔻�肪�ړ�����̉��s
		TraceBoxSize / 2.f, // �����蔻��̑傫��
		GetHeroCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(), // �����蔻�肪�i�ތ���
		BoxTraceChannel, // �K�p�Ώۂ̗�(Pawn�Ȃ�)
		false, // ���G�Փ˂��s����(false����)
		TArray<AActor*>(), // ���肩�珜�O����Actor�̔z��
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None, // �f�o�b�O�\���̎��
		BoxTraceHits, // �q�b�g�̌���(����)������
		true // ���g�������Ŗ������邩
	);

	// UKismetSystemLibrary::BoxTraceMultiForObjects()�̏����̌��ʂ���q�b�g����Actor�𒲂ׂ�
	// HitActor�����g�łȂ���΁A�^�[�Q�b�g���b�N�\�Ƃ��ċL�^����
	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			// �f�o�b�O�p�̏����@!AvailableActorsToLock.Contains(HitActor)�͕s�v
			if (HitActor != GetHeroCharacterFromActorInfo() && !AvailableActorsToLock.Contains(HitActor))
			{
				AvailableActorsToLock.AddUnique(HitActor);
				// Debug::Print(HitActor->GetActorNameOrLabel());
			}
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = 0.f;

	// �����h�L�������g
	// Returns an Actor nearest to Origin from ActorsToCheck array.
	// �^�[�Q�b�g���b�N�\�ȑΏۂ��玩�g�ɍł��߂����̂����߂�B����͋����͕K�v�����̂œK���Ȓl�Ŗ��߂�
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(), InAvailableActors, ClosestDistance);
}

// �Ώۂ�؂�ւ���ۂɑΏۂ����E�ǂ���ɂ��邩�m�F���鏈��
void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight)
{
	// Lock�Ώۂ����Ȃ��ꍇ�͏I������
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	const FVector PlayerLocation = GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized = (CurrentLockedActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

	// Lock�\�ȑΏۂɂ��č��E�ǂ���ɂ��邩�m�F����
	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		// Lock�\�łȂ��B�Ώۂ����݃��b�N���Ȃ珈�����Ȃ�
		if (!AvailableActor || AvailableActor == CurrentLockedActor) continue;

		const FVector PlayerToAvailableNormalized = (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

		const FVector CrossResult = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);

		if (CrossResult.Z > 0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forgot to assign a valid widget class in BP"));

		DrawnTargetLockWidget = CreateWidget<UBluehorseWidgetBase>(GetHeroControllerFromActorInfo(), TargetLockWidgetClass);

		check(DrawnTargetLockWidget);

		DrawnTargetLockWidget->AddToViewport();
	}
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}

	FVector2D ScreenPosition;
	// �����h�L�������g
	// Gets the projected world to screen position for a player,
	// then converts it into a widget position, which takes into account any quality scaling.
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetHeroControllerFromActorInfo(), // �ǂ̃v���C���[���_�ō��W�����߂邩
		CurrentLockedActor->GetActorLocation(), // �\�����郏�[���h��ԏ�̍��W
		ScreenPosition, // �o�͗p��FVector2D�ϐ�
		true // �r���[�|�[�g���ł̑��Έʒu��Ԃ��悤�ɐݒ�
	);

	// Widget�̑傫�����l�����ĕ`�悵�Ȃ��ƈʒu������Ă��܂�
	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		// Widget���̗v�f�𑖍����āASizeBox�̕��ƍ����ɃA�N�Z�X���l��ێ�����
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
			[this](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			}
		);
	}

	// SizeBox�̑傫������`��ʒu�𒲐�����
	ScreenPosition -= (TargetLockWidgetSize / 2.f);
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition, false);

}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->AddMappingContext(TargetLockMappingContext, 3);
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void UHeroGameplayAbility_TargetLock::CleanUp()
{
	AvailableActorsToLock.Empty();

	// CachedHeroCharacter = nullptr;

	CurrentLockedActor = nullptr;

	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}

	DrawnTargetLockWidget = nullptr;

	TargetLockWidgetSize = FVector2D::ZeroVector;

	CachedDefaultMaxwalkSpeed = 0.f;
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	if (!GetHeroControllerFromActorInfo())
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->RemoveMappingContext(TargetLockMappingContext);
}
