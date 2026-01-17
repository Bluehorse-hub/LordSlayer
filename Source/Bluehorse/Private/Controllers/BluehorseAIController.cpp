// Shungen All Rights Reserved.


#include "Controllers/BluehorseAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "BluehorseDebugHelper.h"

/**
 * AIController（敵AI想定）。
 *
 * - PathFollowingComponent を UCrowdFollowingComponent に差し替え、
 *   Detour Crowd（群衆回避）を利用できるようにする
 * - AIPerception（Sight）でターゲットを検知し、Blackboard に反映する
 * - GenericTeamId により、IGenericTeamAgentInterface の敵味方判定を行う
 */

ABluehorseAIController::ABluehorseAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	// --- Sight（視覚）設定 ----------------------------------------------------
	// Enemy 用の視覚センス設定を作成
	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");

	// 検知対象の所属（敵のみ検知）
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;

	// 視覚パラメータ
	AISenseConfig_Sight->SightRadius = 5000.f;
	AISenseConfig_Sight->LoseSightRadius = 0.f;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;

	// --- Perception Component ------------------------------------------------
	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);

	// DominantSense（主センス）を Sight に設定
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());

	// 感知更新イベント（見つけた/見失った等）を受け取る
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);

	// チームID設定（敵AIを 1 として扱う）
	SetGenericTeamId(FGenericTeamId(1));
}

/**
 * 敵味方判定。
 *
 * - Other の Controller が IGenericTeamAgentInterface を実装していれば TeamId を比較する
 * - 現在のロジックは「OtherTeamId < MyTeamId の場合は Hostile、それ以外は Friendly」
 */
ETeamAttitude::Type ABluehorseAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* PawnToCheck = Cast<const APawn>(&Other);

	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());

	if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() < GetGenericTeamId())
	{
		return ETeamAttitude::Hostile;
	}

	return ETeamAttitude::Friendly;
}

void ABluehorseAIController::BeginPlay()
{
	Super::BeginPlay();

	// --- Detour Crowd（群衆回避）設定 ----------------------------------------
	// PathFollowingComponent を CrowdFollowing として使っている場合のみ有効
	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		// Detour Crowd の有効/無効
		CrowdComp->SetCrowdSimulationState(bEnableDetourCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState::Disabled);

		// 確認用コンソールコマンド:ai.crowd.Debug.SelectedActors 1
		switch (DetourCrowdAvoidanceQuality)
		{
			case 1: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);    break;
			case 2: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium); break;
			case 3: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);   break;
			case 4: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);   break;
			default:
				break;
		}

		// 回避グループ設定（同グループ同士を避ける等の運用）
		CrowdComp->SetAvoidanceGroup(1);
		CrowdComp->SetGroupsToAvoid(1);

		// 回避の衝突クエリ範囲（大きいほど早めに回避しやすいがコスト増）
		CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange);
	}
}

/**
 * Perception 更新時に呼ばれる。
 *
 * - Blackboard の "TargetActor" が未設定の場合のみ、感知成功した Actor をセットする
 * - ここでは「最初に見つけたターゲットを固定」する挙動になっている
 *   （ターゲット切替をしたい場合は別ロジックにする）
 */
void ABluehorseAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		// すでにターゲットがいるなら更新しない（固定運用）
		if (!BlackboardComponent->GetValueAsObject(FName("TargetActor")))
		{
			if (Stimulus.WasSuccessfullySensed() && Actor)
			{
				BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
			}
		}
	}
}
