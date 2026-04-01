#include "MissionManager.h"

#include "DroneProject/Missions/Checkpoints/CheckpointMission.h"
#include "DroneProject/Missions/Recons/ReconMission.h"
#include "DroneProject/Missions/Artillery/ArtilleryMission.h"
#include "DroneProject/Missions/Final/FinalMission.h"
#include "DroneProject/Pawn/DronePawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"         // ✅ нужен для GetTimerManager / SetTimer
#include "Engine/World.h"         // ✅ на всякий, чтобы GetWorld точно был

AMissionManager::AMissionManager()
{
    PrimaryActorTick.bCanEverTick = false;

    CheckpointMission = nullptr;
    ReconMission = nullptr;
    ArtilleryMission = nullptr;
    FinalMission = nullptr;
}

void AMissionManager::BeginPlay()
{
    Super::BeginPlay();

    if (ArtilleryMission)
    {
        ArtilleryMission->OnMissionCompleted.RemoveAll(this);
        ArtilleryMission->OnMissionCompleted.AddDynamic(this, &AMissionManager::OnArtilleryMissionCompleted);
    }

    ADronePawn* Drone = Cast<ADronePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    if (Drone)
    {
        Drone->OnDroneDestroyed.RemoveAll(this);
        Drone->OnDroneDestroyed.AddDynamic(this, &AMissionManager::HandleDroneDestroyed);
    }

    if (CheckpointMission)
    {
        CheckpointMission->OnMissionCompleted.RemoveAll(this);
        CheckpointMission->OnMissionCompleted.AddDynamic(this, &AMissionManager::OnCheckpointMissionCompleted);
    }


    if (ReconMission)
    {
        ReconMission->OnMissionCompleted.RemoveAll(this);
        ReconMission->OnMissionCompleted.AddDynamic(this, &AMissionManager::OnReconMissionCompleted);
    }

  
    if (FinalMission)
    {
        FinalMission->OnMissionCompleted.RemoveAll(this);
        FinalMission->OnMissionCompleted.AddDynamic(this, &AMissionManager::OnFinalMissionCompleted);
    }
}

void AMissionManager::StartCheckpointMission()
{
    if (!CheckpointMission) return;

    CurrentMissionType = EMissionType::Checkpoint;
    CheckpointMission->StartMission();
}

void AMissionManager::OnCheckpointMissionCompleted()
{
    if (!ReconMission) return;

    CurrentMissionType = EMissionType::Recon;

    ReconMission->SaveMissionStartFromDrone();
    ReconMission->StartMission();
}

void AMissionManager::OnReconMissionCompleted()
{
    if (!ArtilleryMission) return;

    CurrentMissionType = EMissionType::Artillery;

    ArtilleryMission->SaveMissionStartFromDrone();
    ArtilleryMission->StartMission();             
}



void AMissionManager::HandleDroneDestroyed()
{
    if (!GetWorld()) return;

    GetWorld()->GetTimerManager().ClearTimer(RestartActiveMissionTimer);

    if (CurrentMissionType == EMissionType::Checkpoint && CheckpointMission)
    {
        GetWorld()->GetTimerManager().SetTimer(RestartActiveMissionTimer,CheckpointMission,&ACheckpointMission::ResetMission,3.0f,false);
    }
    else if (CurrentMissionType == EMissionType::Recon && ReconMission)
    {
        GetWorld()->GetTimerManager().SetTimer(RestartActiveMissionTimer,this,&AMissionManager::RestartReconMission,3.0f,false
        );
    }
    
    else if (CurrentMissionType == EMissionType::Artillery && ArtilleryMission)
    {
        GetWorld()->GetTimerManager().SetTimer(RestartActiveMissionTimer,this,&AMissionManager::RestartArtilleryMission,3.0f,false);
    }
    else if (CurrentMissionType == EMissionType::Final && FinalMission)
    {
       
        GetWorld()->GetTimerManager().SetTimer(RestartActiveMissionTimer,this,&AMissionManager::RestartFinalMission,3.0f,false);
    }
}

void AMissionManager::RestartReconMission()
{
    if (!ReconMission) return;

    ADronePawn* Drone = Cast<ADronePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    if (!Drone) return;

    Drone->RespawnDrone(ReconMission->MissionStartLocation, ReconMission->MissionStartRotation);
    ReconMission->StartMission();
}

void AMissionManager::RestartArtilleryMission()
{
    if (!ArtilleryMission) return;

    ADronePawn* Drone = Cast<ADronePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    if (!Drone) return;

    Drone->RespawnDrone(ArtilleryMission->MissionStartLocation, ArtilleryMission->MissionStartRotation);
    ArtilleryMission->ResetMission();
}

void AMissionManager::RestartFinalMission()
{
    if (!FinalMission) return;

  
    FinalMission->StartMission();
}

void AMissionManager::OnArtilleryMissionCompleted()
{
    if (!FinalMission) return;

    CurrentMissionType = EMissionType::Final;

    UWorld* World = GetWorld();
    if (!World) return;

    World->GetTimerManager().ClearTimer(StartFinalMissionTimer);

    World->GetTimerManager().SetTimer(StartFinalMissionTimer,this,&AMissionManager::StartFinalMissionDelayed,2.5f,false);
}

void AMissionManager::StartFinalMissionDelayed()
{
    if (FinalMission)
    {
        FinalMission->StartMission();
    }
}

void AMissionManager::OnFinalMissionCompleted()
{
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);

    UKismetSystemLibrary::QuitGame(World,PC,EQuitPreference::Quit,false);
}