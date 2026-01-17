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
#include "AbilitySystem/Attribute/HajikiAttributeSet.h"
#include "BluehorseGameInstance.h"
#include "BluehorseFunctionLibrary.h"

#include "BluehorseDebugHelper.h"

// HeroCharacter のコンストラクタ
ABluehorseHeroCharacter::ABluehorseHeroCharacter()
{
	// 標準的なプレイヤーカプセルサイズ
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// Controller の回転をそのまま Character に反映しない（移動方向やカメラで制御）
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// --- Camera -------------------------------------------------------------
	// 追従カメラ用 SpringArm（カメラ距離やオフセット、回転追従を管理）
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f);
	CameraBoom->bUsePawnControlRotation = true;

	// 実カメラ（SpringArm の先に接続）
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// --- Interact Detection -------------------------------------------------
	// インタラクト対象の近接検知（Overlap によるフォーカス通知用）
	InteractDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractDetectionSphere"));
	InteractDetectionSphere->SetupAttachment(RootComponent);
	InteractDetectionSphere->SetSphereRadius(200.f); // 検知範囲
	InteractDetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractDetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractDetectionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	// プロジェクト内で「Projectile などが無視すべき対象」を識別するためのタグ
	InteractDetectionSphere->ComponentTags.Add(FName(TEXT("IgnoreProjectile")));

	// --- Movement -----------------------------------------------------------
	// 入力方向へ回転して走るスタイル
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 800.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1500.f;

	// --- Components ---------------------------------------------------------
	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// 追加の AttributeSet（弾き等の独自ステータス）
	HajikiAttributeSet = CreateDefaultSubobject<UHajikiAttributeSet>(TEXT("HajikiAttributeSet"));
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

// Controller に所持されたタイミング
void ABluehorseHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// GameInstanceを取得（武器変更用にデータを取得するため）
	if (UBluehorseGameInstance* BGI = UBluehorseFunctionLibrary::GetBluehorseGameInstance(this))
	{
		// 初期アビリティを同期ロードする処理
		// 旧処理なので不要かもしれないが、安定動作のために一旦残す（要改善）
		if (!CharacterStartUpData.IsNull())
		{
			// 選択中キャラクターのスタートアップデータを同期ロード
			if (UDataAsset_StartUpDataBase* LoadedData = BGI->SelectedCharacterStartupData.LoadSynchronous())
			{
				LoadedData->GiveToAbilitySystemComponent(BluehorseAbilitySystemComponent);
			}
		}
	}
}
// Enhanced Input のセットアップ
void ABluehorseHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	checkf(Subsystem, TEXT("Forgot to assign a valid data asset as input config"));

	// デフォルトの入力マッピングを適用
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	UBluehorseInputComponent* BluehorseInputComponent = CastChecked<UBluehorseInputComponent>(PlayerInputComponent);

	// 移動・視点
	BluehorseInputComponent->BindNativeInputAction(InputConfigDataAsset, BluehorseGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	BluehorseInputComponent->BindNativeInputAction(InputConfigDataAsset, BluehorseGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

	// ターゲット切り替え（押している間に方向入力を取り、離したタイミングでイベント送信）
	BluehorseInputComponent->BindNativeInputAction(InputConfigDataAsset, BluehorseGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Triggered, this, &ThisClass::Input_SwitchTargetTriggered);
	BluehorseInputComponent->BindNativeInputAction(InputConfigDataAsset, BluehorseGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Completed, this, &ThisClass::Input_SwitchTargetCompleted);

	// Ability 入力（InputTag を ASC に渡して Ability を起動/キャンセルする）
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

	// Interact対象へ Focus を送るための Overlap ハンドラ
	InteractDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABluehorseHeroCharacter::OnInteractableBeginOverlap);
	InteractDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ABluehorseHeroCharacter::OnInteractableEndOverlap);
}

// Move 入力
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

// Look 入力（視点操作）
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

// ターゲット切り替え（押下中）
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

// Ability 入力押下：ASC に中継（InputTag → AbilitySpec の解決）
void ABluehorseHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	BluehorseAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

// Ability 入力解放：押しっぱなし系（MustBeHeld）などの終了に使用
void ABluehorseHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	BluehorseAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}
// インタラクト対象が検知範囲に入ったとき。
// InteractableInterface 実装 Actor に Focus 開始を通知する
void ABluehorseHeroCharacter::OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_OnBeginFocus(OtherActor, this);
	}
}

// インタラクト対象が検知範囲から出たとき。
// InteractableInterface 実装 Actor に Focus 終了を通知する
void ABluehorseHeroCharacter::OnInteractableEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_OnEndFocus(OtherActor, this);
	}
}
