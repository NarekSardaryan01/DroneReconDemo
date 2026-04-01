#include "ArtilleryMission.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

#include "DroneProject/Pawn/DronePawn.h"
#include "DroneProject/AntiDroneSystems/AntiAirTurret.h"
#include "DroneProject/Missions/Artillery/ArtilleryTarget.h"

AArtilleryMission::AArtilleryMission()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AArtilleryMission::BeginPlay()
{
    Super::BeginPlay();

    CollectTargets();

    for (AArtilleryTarget* T : Targets)
    {
        if (T)
        {
            T->DeactivateTarget();
        }
    }
}

void AArtilleryMission::CollectTargets()
{
    Targets.Empty();

    for (TActorIterator<AArtilleryTarget> It(GetWorld()); It; ++It)
    {
        AArtilleryTarget* T = *It;
        if (T)
        {
            Targets.Add(T);

            T->OnTargetCompleted.RemoveAll(this);
            T->OnTargetCompleted.AddDynamic(this, &AArtilleryMission::HandleTargetCompleted);
        }
    }
}

void AArtilleryMission::SaveMissionStartFromDrone()
{
    ADronePawn* Drone = Cast<ADronePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    if (!Drone) return;

    MissionStartLocation = Drone->GetActorLocation();
    MissionStartRotation = Drone->GetActorRotation();
}

void AArtilleryMission::StartMission()
{
    CollectTargets();

    bMissionActive = true;
    RemainingTargets = 0;



    if (MissionStartSound)
    {
        UGameplayStatics::PlaySound2D(this, MissionStartSound);
    }

    if (TeachSound4)
    {
        GetWorld()->GetTimerManager().SetTimer(TeachSound4Timer,this,&AArtilleryMission::PlayTeachSound4,1.0f,false);
    }

  

    for (AArtilleryTarget* T : Targets)
    {
        if (T)
        {
            T->ActivateTarget();
            RemainingTargets++;
        }
    }


    if (RemainingTargets <= 0)
    {
        bMissionActive = false;
        OnMissionCompleted.Broadcast();
    }
}

void AArtilleryMission::ResetMission()
{
    bMissionActive = false;

    CollectTargets();

    for (AArtilleryTarget* T : Targets)
    {
        if (T)
        {
       
            if (AAntiAirTurret* Turret = T->GetLinkedTurret())
            {
                Turret->ResetTurret();
            }

            T->DeactivateTarget();
        }
    }

    CurrentHoldTime = 0.f;
    HoldProgress = 0.f;

    StartMission();
}

void AArtilleryMission::HandleTargetCompleted(AArtilleryTarget* Target)
{
    if (!bMissionActive) return;

    RemainingTargets = FMath::Max(RemainingTargets - 1, 0);



    if (RemainingTargets <= 0)
    {
        bMissionActive = false;
        OnMissionCompleted.Broadcast();
    }
}

void AArtilleryMission::UpdateHoldProgress(float DeltaTime, bool bHit)
{
    if (!bMissionActive) return;

    if (bHit)
    {
        CurrentHoldTime += DeltaTime;
    }
    else
    {
        CurrentHoldTime = 0.f;
    }

    HoldProgress = FMath::Clamp(CurrentHoldTime / RequiredHoldTime, 0.f, 1.f);
}

void AArtilleryMission::PlayTeachSound4()
{
    if (TeachSound4)
    {
        UGameplayStatics::PlaySound2D(
            this,
            TeachSound4,
            2.0f
        );
    }
}

