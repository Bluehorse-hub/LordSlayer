// Shungen All Rights Reserved.


#include "Interact/BluehorseBaseInteract.h"

/**
 * インタラクト可能オブジェクトの基底クラス。
 * - 実際の挙動（扉を開く、宝箱を開く等）は派生クラスで実装する
 */
ABluehorseBaseInteract::ABluehorseBaseInteract()
{
	// Tick は不要なため無効化
	PrimaryActorTick.bCanEverTick = false;

	// 見た目兼インタラクト判定用のメッシュ
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	// 物理衝突は行わず、クエリのみ有効
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// すべてのチャンネルを無効化
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Visibility のみ Block
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Mesh を RootComponent として使用
	RootComponent = Mesh;
}
