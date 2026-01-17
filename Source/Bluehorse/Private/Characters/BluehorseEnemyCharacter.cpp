// Shungen All Rights Reserved.


#include "Characters/BluehorseEnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "Engine/AssetManager.h"
#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/BluehorseWidgetBase.h"

#include "BluehorseDebugHelper.h"

// EnemyCharacter のコンストラクタ
ABluehorseEnemyCharacter::ABluehorseEnemyCharacter()
{
	// 配置 or Spawn 時に自動で AIController に所持させる
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Controller の回転は直接使わず、MovementComponent に委ねる
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// カメラとの不要な衝突を防ぐ
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// Enemy の移動設定（AI 操作前提）
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	// Enemy 専用 CombatComponent
	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");

	// Enemy 専用 UI コンポーネント
	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>("EnemyUIComponent");

	// 敵HP表示用 WidgetComponent
	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyHealthWidgetComponent");
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());
}

// 戦闘コンポーネント取得
UPawnCombatComponent* ABluehorseEnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

// Pawn 共通 UI コンポーネント取得
UPawnUIComponent* ABluehorseEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

// Enemy 専用 UI コンポーネント取得。
// Enemy 固有処理（HPバー等）に使用する。
UEnemyUIComponent* ABluehorseEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

void ABluehorseEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UBluehorseWidgetBase* HealthWidget = Cast<UBluehorseWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject()))
	{
		HealthWidget->InitEnemyCreatedWidget(this);
	}
}

// AIController に所持されたタイミングで呼ばれる
void ABluehorseEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitEnemyStartUpData();
}

// Enemy 用 StartUpData を AbilitySystemComponent に適用する
void ABluehorseEnemyCharacter::InitEnemyStartUpData()
{
	// StartUpData が未設定なら何もしない
	if (CharacterStartUpData.IsNull())
	{
		return;
	}

	// 非同期ロードで StartUpData を取得
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
				{
					// Ability / GameplayEffect / 初期タグなどを ASC に付与
					LoadedData->GiveToAbilitySystemComponent(BluehorseAbilitySystemComponent);
				}
			}
		)
	);
}
