// Shungen All Rights Reserved.


#include "Interact/BluehorseDoorInteract.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"

ABluehorseDoorInteract::ABluehorseDoorInteract()
{
	LeftDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMesh"));

	LeftDoorMesh->SetupAttachment(Mesh);

	RightDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorMesh"));

	RightDoorMesh->SetupAttachment(Mesh);

    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionObjectType(ECC_WorldStatic);
    Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    LeftDoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    LeftDoorMesh->SetCollisionObjectType(ECC_WorldDynamic);
    LeftDoorMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    LeftDoorMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    LeftDoorMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    LeftDoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    RightDoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    RightDoorMesh->SetCollisionObjectType(ECC_WorldDynamic);
    RightDoorMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    RightDoorMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    RightDoorMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    RightDoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ABluehorseDoorInteract::BeginPlay()
{
    Super::BeginPlay();

    // 再生設定（自動再生はしない）
    FMovieSceneSequencePlaybackSettings Settings;
    Settings.bAutoPlay = false;

    // LevelSequencePlayer / Actor を生成
    // ※ 毎回 Create すると Transform や状態が不安定になるため、
    //    BeginPlay で一度だけ生成して使い回す
    DoorSequencePlayer =
        ULevelSequencePlayer::CreateLevelSequencePlayer(
            GetWorld(),
            OpenSequence,
            Settings,
            DoorSequenceActor
        );

    // Create に失敗した場合の安全チェック
    if (!DoorSequencePlayer || !DoorSequenceActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Door] Failed to create SequencePlayer or SequenceActor"));
        return;
    }

    // Sequencer 内の Object Binding に付けたタグ "Door" に、
    // このドア Actor（this）を割り当てる
    // これにより BP_Door / BP_Door2 など名前に依存せず、
    // 再生時に「操作中のドア」だけを確実に制御できる
    static const FName DoorBindingTag(TEXT("Door"));
    DoorSequenceActor->SetBindingByTag(DoorBindingTag, { this }, false);
}

void ABluehorseDoorInteract::RequestToggle()
{
    PlayOpenDoorSequence();
}

void ABluehorseDoorInteract::PlayOpenDoorSequence()
{
    // すでに開いている場合は再生しない
    if (IsOpen)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Door] Door is already opened"));
        return;
    }

    // BeginPlay で生成済みの SequencePlayer を再生
    // （毎回 Create しないことで消失・瞬間移動を防ぐ）
    if (DoorSequencePlayer)
    {
        DoorSequencePlayer->Play();
        IsOpen = true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Door] SequencePlayer is NULL"));
    }
}
