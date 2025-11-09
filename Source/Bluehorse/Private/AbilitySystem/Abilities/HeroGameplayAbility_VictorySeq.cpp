// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/HeroGameplayAbility_VictorySeq.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"

void UHeroGameplayAbility_VictorySeq::SpawnVictoryCamera()
{
	// カメラクラスが未設定の場合は処理を中断
	if (!VictoryCameraClass)
	{
		return;
	}

	// スポーン設定
    FActorSpawnParameters Params;
    Params.Owner = GetAvatarActorFromActorInfo(); // 所有者をAbilityのAvatar（キャラ）に設定
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// カメラの初期配置を計算（キャラの後方＆上方）
    AActor* OwnerActor = GetAvatarActorFromActorInfo();
    FVector CamLoc = OwnerActor->GetActorLocation() - OwnerActor->GetActorForwardVector() * 300.f + FVector(0.f, 200.f, 150.f);

	// 少し俯瞰になるように回転を調整（キャラの背後180°）
    FRotator CamRot = OwnerActor->GetActorRotation() + FRotator(-10.f, 180.f, 0.f);

	// カメラをスポーン
    VictoryCameraActor = GetWorld()->SpawnActor<ACineCameraActor>(VictoryCameraClass, CamLoc, CamRot, Params);
}

void UHeroGameplayAbility_VictorySeq::SettingVictoryCamera(float CurrentFocalLength, float CurrentAperture, AActor* ActorToTrack)
{
	if (VictoryCameraActor)
	{
		// 演出中に敵などを常に追尾させるための設定
		VictoryCameraActor->LookatTrackingSettings.bEnableLookAtTracking = true;
		VictoryCameraActor->LookatTrackingSettings.ActorToTrack = ActorToTrack;

		UCineCameraComponent* CineComp = VictoryCameraActor->GetCineCameraComponent();

		if (CineComp)
		{
			// アスペクト比を強制調整しない（ズーム補正を防ぐ）
			CineComp->bConstrainAspectRatio = false;

			// ズーム固定（焦点距離を手動で指定）
			CineComp->SetCurrentFocalLength(CurrentFocalLength);
			CineComp->CurrentAperture = CurrentAperture;

			// DOFを手動制御に変更（Trackingによるズームを防ぐ）
			CineComp->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;
		}

		// 初期状態では画面に表示しない（Blend切り替え時にONにする）
		VictoryCameraActor->SetActorHiddenInGame(true);

		UE_LOG(LogTemp, Log, TEXT("[HeroCharacter] VictoryCameraActor spawned successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[HeroCharacter] Failed to spawn VictoryCameraActor."));
	}
}
