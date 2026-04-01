#include "CheckpointMission.h"
#include "CheckpointActor.h"
#include "DroneProject/Pawn/DronePawn.h"
#include "Kismet/GameplayStatics.h"

ACheckpointMission::ACheckpointMission()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ACheckpointMission::BeginPlay()
{
    Super::BeginPlay();

    for (ACheckpointActor* CP : Checkpoints)
    {
        if (CP)
        {
            CP->DeactivateCheckpoint();
            CP->OnCheckpointPassed.RemoveAll(this);
        }
    }
}

void ACheckpointMission::StartMission()
{
    if (Checkpoints.Num() == 0) return;

    if (MissionStartSound)
    {
        UGameplayStatics::PlaySound2D(this, MissionStartSound);
    }


    GetWorld()->GetTimerManager().ClearTimer(MissionTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);

    for (ACheckpointActor* CP : Checkpoints)
    {
        if (CP)
        {
            CP->DeactivateCheckpoint();
            CP->OnCheckpointPassed.RemoveAll(this);
        }
    }

    CurrentCheckpointIndex = 0;

    CachedDrone = Cast<ADronePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

    if (CachedDrone)
    {
        SavedDroneLocation = CachedDrone->GetActorLocation();
        SavedDroneRotation = CachedDrone->GetActorRotation();

        CachedDrone->OnDroneCrashed.RemoveAll(this);
        CachedDrone->OnDroneCrashed.AddDynamic(
            this,
            &ACheckpointMission::HandleDroneCrash
        );
    }


    if (Checkpoints[0])
    {
        Checkpoints[0]->ActivateCheckpoint();
        Checkpoints[0]->OnCheckpointPassed.AddDynamic(this,&ACheckpointMission::OnCheckpointPassed);
    }


    RemainingTime = MissionDuration;


    GetWorld()->GetTimerManager().SetTimer(MissionTimerHandle,this,&ACheckpointMission::HandleTimeExpired,MissionDuration,false);
    GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle,this,&ACheckpointMission::UpdateCountdown,1.f,true);


}

void ACheckpointMission::UpdateCountdown()
{
    RemainingTime--;

    if (RemainingTime <= 0.f)
    {
        RemainingTime = 0.f;
        GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
    }
}

void ACheckpointMission::OnCheckpointPassed(ACheckpointActor* PassedCheckpoint)
{
    if (!PassedCheckpoint) return;

    PassedCheckpoint->DeactivateCheckpoint();
    PassedCheckpoint->OnCheckpointPassed.RemoveAll(this);

    CurrentCheckpointIndex++;

    if (Checkpoints.IsValidIndex(CurrentCheckpointIndex))
    {
        ACheckpointActor* Next = Checkpoints[CurrentCheckpointIndex];

        if (Next)
        {
            Next->ActivateCheckpoint();
            Next->OnCheckpointPassed.AddDynamic(
                this,
                &ACheckpointMission::OnCheckpointPassed
            );
        }
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(MissionTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
        FinishMission();     
        OnMissionCompleted.Broadcast();
    }
}

void ACheckpointMission::HandleTimeExpired()
{
    ResetMission();
}

void ACheckpointMission::HandleDroneCrash()
{
    GetWorld()->GetTimerManager().ClearTimer(CrashDelayHandle);
    GetWorld()->GetTimerManager().SetTimer(CrashDelayHandle,this,&ACheckpointMission::ResetMission,1.5f,false);
}

void ACheckpointMission::ResetMission()
{
    GetWorld()->GetTimerManager().ClearTimer(MissionTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);

    for (ACheckpointActor* CP : Checkpoints)
    {
        if (CP)
        {
            CP->DeactivateCheckpoint();
            CP->OnCheckpointPassed.RemoveAll(this);
        }
    }

    if (CachedDrone)
    {
        CachedDrone->RespawnDrone(
            SavedDroneLocation,
            SavedDroneRotation
        );
    }

    StartMission();
}

void ACheckpointMission::FinishMission()
{

    GetWorld()->GetTimerManager().ClearTimer(MissionTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);


    for (ACheckpointActor* CP : Checkpoints)
    {
        if (CP)
        {
            CP->DeactivateCheckpoint();
            CP->OnCheckpointPassed.RemoveAll(this);
        }
    }


    if (CachedDrone)
    {
        CachedDrone->OnDroneCrashed.RemoveAll(this);
    }
}