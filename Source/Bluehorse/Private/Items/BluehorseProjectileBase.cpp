// Shungen All Rights Reserved.


#include "Items/BluehorseProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "BluehorseDebugHelper.h"

ABluehorseProjectileBase::ABluehorseProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 4.f;
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

void ABluehorseProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// �ՓˑΏۂ�Pawn���ǂ������m�F
	APawn* HitPawn = Cast<APawn>(OtherActor);

	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

	// �G�Ί֌W�ɂȂ��ꍇ�̓_���[�W�������s�킸�I���i�����△�֌W�I�u�W�F�N�g�ւ̌�q�b�g�h�~�j
	if (!HitPawn || !UBluehorseFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		Destroy();
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
	Destroy();
}

void ABluehorseProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Projectile���������Ăق����Ȃ��R���|�[�l���g�𖳎�
	// ����ɂ��A�C���^���N�g�pSphere�Ȃǂ�Overlap�����O�ł���
	if (OtherComp && OtherComp->ComponentHasTag(TEXT("IgnoreProjectile")))
	{
		UE_LOG(LogTemp, Verbose, TEXT("Ignored projectile overlap with %s (tagged IgnoreProjectile)"), *OtherComp->GetName());
		return;
	}

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
			// Projectile�ɕR�Â�GameplayEffectSpecHandle���g���ă_���[�W��K�p
			HandleApplyProjectileDamage(HitPawn, Data);

			// �������Projectile��j��
			// �V���O���q�b�g�^��Projectile�Ƃ��Ĕj��
			Destroy();
		}
	}
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

