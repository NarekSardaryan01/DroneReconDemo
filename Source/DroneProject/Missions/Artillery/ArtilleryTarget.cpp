#include "ArtilleryTarget.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "EngineUtils.h"
#include "DroneProject/AntiDroneSystems/AntiAirTurret.h"
#include "DroneProject/DroneHelpers/ArtilleryShell.h"

AArtilleryTarget::AArtilleryTarget()
{
    PrimaryActorTick.bCanEverTick = false;

  
    DesignateCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DesignateCollision"));
    RootComponent = DesignateCollision;

    DesignateCollision->SetBoxExtent(FVector(80.f, 80.f, 120.f)); // подгони под турель
    DesignateCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DesignateCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    DesignateCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    DesignateCollision->SetGenerateOverlapEvents(false);
    
    VisualCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualCylinder"));
    VisualCylinder->SetupAttachment(RootComponent);
    VisualCylinder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualCylinder->SetGenerateOverlapEvents(false);
    VisualCylinder->SetHiddenInGame(false);


    bActive = false;

    HoldTimer = 0.f;
}

void AArtilleryTarget::BeginPlay()
{
    Super::BeginPlay();

    bActive = false;
 
    HoldTimer = 0.f;

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    if (VisualCylinder)
    {
        VisualCylinder->SetHiddenInGame(true);
    }


    if (!LinkedTurret)
    {
        float BestDistSq = FLT_MAX;
        AAntiAirTurret* BestTurret = nullptr;

        for (TActorIterator<AAntiAirTurret> It(GetWorld()); It; ++It)
        {
            AAntiAirTurret* Turret = *It;
            if (!Turret) continue;

            float Dist = FVector::DistSquared(Turret->GetActorLocation(),GetActorLocation()
            );

            if (Dist < BestDistSq)
            {
                BestDistSq = Dist;
                BestTurret = Turret;
            }
        }

        LinkedTurret = BestTurret;

    }
}

void AArtilleryTarget::ActivateTarget()
{
    bActive = true;
    HoldTimer = 0.f;

    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);

    if (VisualCylinder)
    {
        VisualCylinder->SetHiddenInGame(false);
    }

}

void AArtilleryTarget::DeactivateTarget()
{
    bActive = false;
    HoldTimer = 0.f;

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    if (VisualCylinder)
    {
        VisualCylinder->SetHiddenInGame(true);
    }
}

void AArtilleryTarget::SetLinkedTurret(AAntiAirTurret* InTurret)
{
    LinkedTurret = InTurret;
}

void AArtilleryTarget::ProcessDesignate(float DeltaTime, bool bIsLookedAt)
{
    
    if (!bActive) return;

    if (bIsLookedAt)
    {
        HoldTimer += DeltaTime;

        if (HoldTimer >= RequiredHoldTime)
        {
            ConfirmStrike();
        }
    }
    else
    {
        HoldTimer = 0.f;
    }
}

void AArtilleryTarget::ResetHold()
{
    HoldTimer = 0.f;
}

void AArtilleryTarget::ConfirmStrike()
{

    if (!bActive) return;

    bActive = false;
    HoldTimer = 0.f;

    const FVector ImpactLocation = GetActorLocation();

    DeactivateTarget();

    if (MarkFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),MarkFX,ImpactLocation);
    }

    if (MarkSound)
    {
        UGameplayStatics::PlaySound2D(this, MarkSound, MarkSoundVolume);
    }

    SpawnShell(ImpactLocation);  

    OnTargetCompleted.Broadcast(this);
}

void AArtilleryTarget::SpawnShell(const FVector& ImpactLocation)
{
    if (!ArtilleryShellClass) return;

    FVector SpawnLoc = ImpactLocation;
    SpawnLoc.Z += ShellSpawnHeight; 

    FActorSpawnParameters Params;
    Params.Owner = this;

    AArtilleryShell* Shell = GetWorld()->SpawnActor<AArtilleryShell>(ArtilleryShellClass,SpawnLoc,FRotator::ZeroRotator,Params);

    if (Shell)
    {
        Shell->SetImpactPoint(ImpactLocation); 
        Shell->SetTargetTurret(LinkedTurret); 
    }
}