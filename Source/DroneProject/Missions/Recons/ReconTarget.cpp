#include "ReconTarget.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AReconTarget::AReconTarget()
{
    PrimaryActorTick.bCanEverTick = false;

    // ================= ROOT COLLISION =================
    PhotoCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PhotoCollision"));
    RootComponent = PhotoCollision;

    PhotoCollision->SetBoxExtent(FVector(60.f));
    PhotoCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PhotoCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    PhotoCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    PhotoCollision->SetGenerateOverlapEvents(false);

    // ================= VISUAL =================
    VisualCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualCylinder"));
    VisualCylinder->SetupAttachment(RootComponent);

    VisualCylinder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualCylinder->SetGenerateOverlapEvents(false);
    VisualCylinder->SetHiddenInGame(false);
}

void AReconTarget::BeginPlay()
{
    Super::BeginPlay();

    bActive = false;
    CurrentScanTime = 0.f;

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    VisualCylinder->SetHiddenInGame(true);
}

void AReconTarget::ActivateTarget()
{
    bActive = true;
    CurrentScanTime = 0.f;

    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    VisualCylinder->SetHiddenInGame(false);

    UE_LOG(LogTemp, Warning, TEXT("Recon Target Activated: %s"), *GetName());
}

void AReconTarget::DeactivateTarget()
{
    bActive = false;
    CurrentScanTime = 0.f;

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    VisualCylinder->SetHiddenInGame(true);
}

void AReconTarget::ProcessScan(float DeltaTime, bool bHitByScan)
{
    if (!bActive) return;

    if (bHitByScan)
    {
        CurrentScanTime += DeltaTime;

        if (CurrentScanTime >= RequiredScanTime)
        {
            // 🔊 2D snapshot sound
            if (SnapshotSound)
            {
                UGameplayStatics::PlaySound2D(this, SnapshotSound, SnapshotVolume);
            }

            bActive = false;
            DeactivateTarget();
            OnTargetCaptured.Broadcast(this);
        }
    }
    else
    {
        CurrentScanTime = 0.f;
    }
}