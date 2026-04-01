#include "FinalMission.h"

#include "FinalTarget.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

AFinalMission::AFinalMission()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AFinalMission::BeginPlay()
{
    Super::BeginPlay();

    if (FinalTarget)
    {
        FinalTarget->DeactivateTarget();

        FinalTarget->OnTargetTriggered.RemoveAll(this);
        FinalTarget->OnTargetTriggered.AddDynamic(this, &AFinalMission::HandleTargetTriggered);
    }
}

void AFinalMission::StartMission()
{
    if (!FinalTarget) return;

    bMissionActive = true;
    bEndCountdownStarted = false;

    CurrentHoldTime = 0.f;
    HoldProgress = 0.f;

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(TeachSoundTimer);
    }

    if (MissionStartSound)
    {
        UGameplayStatics::PlaySound2D(this, MissionStartSound);
    }

    if (TeachSound5 && GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(TeachSoundTimer,this,&AFinalMission::PlayTeachSound5,TeachSoundDelay,false);
    }

    FinalTarget->ActivateTarget();
}

void AFinalMission::ResetMission()
{
    bMissionActive = false;
    bEndCountdownStarted = false;

    CurrentHoldTime = 0.f;
    HoldProgress = 0.f;

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(TeachSoundTimer);
    }

    if (FinalTarget)
    {
        FinalTarget->DeactivateTarget();
    }
}

void AFinalMission::HandleTargetTriggered(AFinalTarget* Target)
{
    if (!bMissionActive) return;
    if (bEndCountdownStarted) return;

 

    bEndCountdownStarted = true;

    CurrentHoldTime = 0.f;
    HoldProgress = 0.f;
}

void AFinalMission::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bMissionActive) return;
    if (!bEndCountdownStarted) return;

    CurrentHoldTime += DeltaTime;
    HoldProgress = FMath::Clamp(CurrentHoldTime / RequiredHoldTime, 0.f, 1.f);

    if (CurrentHoldTime >= RequiredHoldTime)
    {
        CompleteGame();
    }
}

void AFinalMission::CompleteGame()
{
    bMissionActive = false;
    bEndCountdownStarted = false;

    OnMissionCompleted.Broadcast();
}

void AFinalMission::PlayTeachSound5()
{
    if (TeachSound5)
    {
        UGameplayStatics::PlaySound2D(this, TeachSound5, 2.0f);
    }
}