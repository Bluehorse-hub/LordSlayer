// Shungen All Rights Reserved.


#include "Characters/BluehorseHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/Input/BluehorseInputComponent.h"
#include "BluehorseGameplayTags.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/UI/HeroUIComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interfaces/InteractableInterface.h"
#include "Components/Inventory/InventoryComponent.h"

#include "BluehorseDebugHelper.h"

ABluehorseHeroCharacter::ABluehorseHeroCharacter()
{
	/*PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;*/

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	InteractDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractDetectionSphere"));
	InteractDetectionSphere->SetupAttachment(RootComponent);
	InteractDetectionSphere->SetSphereRadius(200.f); // 検知範囲
	InteractDetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractDetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractDetectionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	InteractDetectionSphere->ComponentTags.Add(FName(TEXT("IgnoreProjectile")));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 800.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1500.f;

	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}


UPawnCombatComponent* ABluehorseHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

UPawnUIComponent* ABluehorseHeroCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;
}

UHeroUIComponent* ABluehorseHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

void ABluehorseHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 初期アビリティを同期ロードする処理
	if (!CharacterStartUpData.IsNull())
	{
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			LoadedData->GiveToAbilitySystemComponent(BluehorseAbilitySystemComponent);
		}
	}
}

void ABluehorseHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	checkf(Subsystem, TEXT("Forgot to assign a valid data asset as input config"));

	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	UBluehorseInputComponent* BluehorseInputComponent = CastChecked<UBluehorseInputComponent>(PlayerInputComponent);

	BluehorseInputComponent->BindNativeInputAction(InputConfigDataAsset, BluehorseGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	BluehorseInputComponent->BindNativeInputAction(InputConfigDataAsset, BluehorseGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

	BluehorseInputComponent->BindNativeInputAction(InputConfigDataAsset, BluehorseGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Triggered, this, &ThisClass::Input_SwitchTargetTriggered);
	BluehorseInputComponent->BindNativeInputAction(InputConfigDataAsset, BluehorseGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Completed, this, &ThisClass::Input_SwitchTargetCompleted);

	BluehorseInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
}

void ABluehorseHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!InteractDetectionSphere)
	{
		UE_LOG(LogTemp, Error, TEXT("InteractDetectionSphere is NULL in BeginPlay for %s!"), *GetName());
		return;
	}

	InteractDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABluehorseHeroCharacter::OnInteractableBeginOverlap);
	InteractDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ABluehorseHeroCharacter::OnInteractableEndOverlap);
}

void ABluehorseHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);

		AddMovementInput(ForwardDirection, MovementVector.Y);
	}

	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABluehorseHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABluehorseHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	SwitchDirection = InputActionValue.Get<FVector2D>();
}

void ABluehorseHeroCharacter::Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		SwitchDirection.X > 0.f ? BluehorseGameplayTags::Player_Event_SwitchTarget_Right : BluehorseGameplayTags::Player_Event_SwitchTarget_Left,
		Data
	);
}

void ABluehorseHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	BluehorseAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void ABluehorseHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	BluehorseAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}

void ABluehorseHeroCharacter::OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_OnBeginFocus(OtherActor, this);
	}
}

void ABluehorseHeroCharacter::OnInteractableEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_OnEndFocus(OtherActor, this);
	}
}
