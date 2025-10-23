// Shungen All Rights Reserved.


#include "Items/BluehorseProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "BluehorseDebugHelper.h"

ABluehorseProjectileBase::ABluehorseProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionProfileName(TEXT("Projectile"));
	//ProjectileCollisionBox->SetCollisionObjectType(ECC_GameTraceChannel1);
	//ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	//ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	//ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	//ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	//ProjectileCollisionBox->SetNotifyRigidBodyCollision(false);
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);
	ProjectileCollisionBox->ComponentTags.Add(FName(TEXT("IgnoreProjectile")));

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());
	ProjectileNiagaraComponent->ComponentTags.Add(FName(TEXT("IgnoreProjectile")));

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->ComponentTags.Add(FName(TEXT("IgnoreProjectile")));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 4.f;
}

void ABluehorseProjectileBase::SetTargetActorFromBBKey(APawn* SourcePawn, FName TargetKeyName)
{
	if (!ProjectileMovementComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetActorFromBBKey: ProjectileMovementComp is null"));
		return;
	}

	if (!IsValid(SourcePawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetActorFromBBKey: SourcePawn is invalid"));
		return;
	}

	// --- Controller�擾�iAI or Player �ǂ���ł��j ---
	AAIController* AIController = Cast<AAIController>(SourcePawn->GetController());
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetActorFromBBKey: Controller is not AI (%s)"), *SourcePawn->GetName());
		return;
	}

	// --- Blackboard�擾 ---
	UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	if (!BBComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetActorFromBBKey: BlackboardComponent is null (%s)"), *SourcePawn->GetName());
		return;
	}

	// --- �w��L�[�Ń^�[�Q�b�g���擾 ---
	UObject* TargetObj = BBComp->GetValueAsObject(TargetKeyName);
	AActor* TargetActor = Cast<AActor>(TargetObj);

	if (!IsValid(TargetActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetActorFromBBKey: Blackboard Key '%s' is invalid"), *TargetKeyName.ToString());
		return;
	}

	// --- �z�[�~���O�ݒ� ---
	ProjectileMovementComp->HomingTargetComponent = TargetActor->GetRootComponent();

	FString DebugMessage = FString::Printf(
		TEXT("SetTargetActorFromBBKey: Homing target set to %s via key '%s' (SourcePawn=%s)"),
		*TargetActor->GetName(),
		*TargetKeyName.ToString(),
		*SourcePawn->GetName()
	);

	//Debug::Print(DebugMessage, FColor::Green);
}

void ABluehorseProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// ��ѓ���ђʂ��邩�ǂ����ȂǂɎg���\��
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

void ABluehorseProjectileBase::Destroyed()
{
	Super::Destroyed();

	// �����Ŕj�������i�����Ȃǁj�Ȃ牽�����Ȃ�
	if (bWasManuallyDestroyed)
	{
		return;
	}

	// �����ŏ��ł����ꍇ�̂ݎ��s
	BP_OnSpawnProjectileExpireFX();

	//Debug::Print(TEXT("Projectile expired naturally after lifespan"), FColor::Green);
}

void ABluehorseProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Projectile���������Ăق����Ȃ��R���|�[�l���g�𖳎�
	// ����ɂ��A�C���^���N�g�pSphere�Ȃǂ�Hit�����O�ł���
	if (OtherComp && OtherComp->ComponentHasTag(TEXT("IgnoreProjectile")))
	{
		UE_LOG(LogTemp, Verbose, TEXT("Ignored projectile overlap with %s (tagged IgnoreProjectile)"), *OtherComp->GetName());

		// --- ��~�h�~���� ---
		if (ProjectileMovementComp)
		{
			// �z�[�~���O�e�Ȃ�^�[�Q�b�g���ێ������܂ܑ��x�ēK�p
			if (ProjectileMovementComp->bIsHomingProjectile && ProjectileMovementComp->HomingTargetComponent.IsValid())
			{
				const FVector ToTarget = (ProjectileMovementComp->HomingTargetComponent->GetComponentLocation() - GetActorLocation()).GetSafeNormal();
				ProjectileMovementComp->Velocity = ToTarget * ProjectileMovementComp->InitialSpeed;
			}
			else
			{
				// ���i�e�Ȃ�i�s�������ێ����đ��x�ēK�p
				const FVector ForwardDir = GetActorForwardVector();
				ProjectileMovementComp->Velocity = ForwardDir * ProjectileMovementComp->InitialSpeed;
			}
		}

		return;
	}
	// �ՓˑΏۂ�Pawn���ǂ������m�F
	APawn* HitPawn = Cast<APawn>(OtherActor);

	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

	// �G�Ί֌W�ɂȂ��ꍇ�̓_���[�W�������s�킸�I���i�����△�֌W�I�u�W�F�N�g�ւ̌�q�b�g�h�~�j
	if (!HitPawn || !UBluehorseFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		HandleManualDestroy();
		return;
	}

	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitPawn, BluehorseGameplayTags::Player_Status_Blocking);


	if (bIsPlayerBlocking)
	{
		// Projectile�̊p�x��ʒu�Ȃǂ���u�L���ȃu���b�N�v���ǂ����𔻒�
		bIsValidBlock = UBluehorseFunctionLibrary::IsValidBlock(this, HitPawn);
	}

	// GameplayEvent�p�̃f�[�^���쐬�iInstigator: Projectile, Target: ��e�ҁj
	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;

	if (bIsValidBlock)
	{
		// �u���b�N�����������ꍇ�AGAS�C�x���g�𔭉�
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitPawn,
			BluehorseGameplayTags::Player_Event_SuccessfulBlock,
			Data
		);
	}
	else
	{
		HandleApplyProjectileDamage(HitPawn, Data);
	}

	// �q�b�g���Projectile��j��
	HandleManualDestroy();
}

void ABluehorseProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap: Self=%s (Tags=%s) | Other=%s | OtherComp=%s (Tags=%s)"),
		*GetName(),
		*FString::JoinBy(ProjectileCollisionBox->ComponentTags, TEXT(","), [](const FName& Tag) { return Tag.ToString(); }),
		*OtherActor->GetName(),
		*OtherComp->GetName(),
		*FString::JoinBy(OtherComp->ComponentTags, TEXT(","), [](const FName& Tag) { return Tag.ToString(); })
	);

	// �����A�N�^�[�ɑ΂��Ă�1�񂾂���������
	if (OverlappedActors.Contains(OtherActor))
	{
		return;
	}

	// �����ς݃��X�g�ɒǉ�
	OverlappedActors.AddUnique(OtherActor);

	// ���肪Pawn�łȂ��ꍇ�͖���
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		// ���G��Ԃ̃L�����N�^�[�̓_���[�W����
		const bool bIsPlayerInvincible = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitPawn, BluehorseGameplayTags::Player_Status_Invincible);

		// ���蔲������ɂȂ�悤�ɁA���̂܂܏I��
		if (bIsPlayerInvincible)
		{
			// ��𐬌����͒ǔ����Ȃ��悤�ɂ���
			if (ProjectileMovementComp)
			{
				ProjectileMovementComp->bIsHomingProjectile = false;
				ProjectileMovementComp->HomingTargetComponent = nullptr;

				// ���x��ۂ����܂ܒ��i������
				const FVector CurrentDirection = ProjectileMovementComp->Velocity.GetSafeNormal();
				ProjectileMovementComp->Velocity = CurrentDirection * ProjectileMovementComp->InitialSpeed;
			}
			return;
		}

		// GameplayEventData���\�z
		// �_���[�W��C�x���g�𑗂�ۂɎg�p����f�[�^�\����
		// Instigator�i���ˎҁj��Target�i��e�ҁj��ݒ�
		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;

		// �G�Ί֌W�̔���
		// �G�Ί֌W������ꍇ�̂݃_���[�W�K�p
		if (UBluehorseFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			BP_OnSpawnProjectileHitFX(SweepResult.ImpactPoint);

			bool bIsValidBlock = false;

			const bool bIsPlayerBlocking = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitPawn, BluehorseGameplayTags::Player_Status_Blocking);


			if (bIsPlayerBlocking)
			{
				// Projectile�̊p�x��ʒu�Ȃǂ���u�L���ȃu���b�N�v���ǂ����𔻒�
				bIsValidBlock = UBluehorseFunctionLibrary::IsValidBlock(this, HitPawn);
			}

			// Projectile�ɕR�Â�GameplayEffectSpecHandle���g���ă_���[�W��K�p
			if (bIsValidBlock)
			{
				// �u���b�N�����������ꍇ�AGAS�C�x���g�𔭉�
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
					HitPawn,
					BluehorseGameplayTags::Player_Event_SuccessfulBlock,
					Data
				);
			}
			else
			{
				HandleApplyProjectileDamage(HitPawn, Data);
			}

			// �������Projectile��j��
			// �V���O���q�b�g�^��Projectile�Ƃ��Ĕj��
			HandleManualDestroy();
		}
	}
}

void ABluehorseProjectileBase::HandleManualDestroy()
{
	// ���R���łł͂Ȃ�Hit�ȂǂŎ蓮�Ŕj�󂵂����ǂ���
	bWasManuallyDestroyed = true;
	Destroy();
}

void ABluehorseProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	// Projectile �ɐݒ肳��Ă��� GameplayEffectSpecHandle ���L�����`�F�b�N
	checkf(ProjectileDamageEffectSpecHandle.IsValid(), TEXT("Forgot to assign a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());

	// Instigator�i���ˎҁj����q�b�g���� Pawn �� GameplayEffect ��K�p
	const bool bWasApplied = UBluehorseFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);

	// ���ʂ̓K�p�����������ꍇ�̂݁A�q�b�g���A�N�V�����C�x���g�𔭉�
	if (bWasApplied)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			BluehorseGameplayTags::Shared_Event_HitReact,
			InPayload
		);
	}
}

