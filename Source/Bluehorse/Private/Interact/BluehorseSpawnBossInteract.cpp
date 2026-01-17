// Shungen All Rights Reserved.


#include "Interact/BluehorseSpawnBossInteract.h"
#include "Kismet/GameplayStatics.h"

/**
 * ボス出現用インタラクト（トリガー）Actor。
 *
 * - Mesh を Overlap トリガーとして使い、プレイヤー等が侵入したらボスを生成する想定
 * - トリガー自体は不可視にして、ゲームプレイ上は存在だけを使う
 * - 生成は一度だけ（IsSpawn で二重生成を防止）
 */
ABluehorseSpawnBossInteract::ABluehorseSpawnBossInteract()
{
	// トリガーとして Overlap を受け取るために設定
	// （BaseInteract 側が Visibility Block 等をしていても、ここで必要なチャンネルを追加）
	Mesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ABluehorseSpawnBossInteract::BeginPlay()
{
	Super::BeginPlay();

	// トリガーは見た目が不要なため不可視化
	if (Mesh)
	{
		Mesh->SetVisibility(false, true);
		Mesh->SetHiddenInGame(true);
	}
}

/**
 * ボスをスポーンする。
 *
 * - すでにスポーン済みの場合は何もしない
 * - SpawnCollisionHandlingOverride を AdjustIfPossibleButAlwaysSpawn にして
 *   多少の干渉があっても生成できるようにしている
 */
void ABluehorseSpawnBossInteract::SpawnBoss()
{
	// 二重スポーン防止
	if (IsSpawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpawnBoss] Already spawn"));
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;

	// 衝突があっても可能な限り調整して生成する
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 実際のスポーン
	AActor* SpawnedBoss = GetWorld()->SpawnActor<AActor>(
		BossClass,
		BossTransform,
		Params
	);

	if (SpawnedBoss)
	{
		// 生成成功：以後スポーンしない
		IsSpawn = true;
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpawnBoss] Spawn FAILED"));
		return;
	}
}
