#include "ReconMission.h"
#include "ReconTarget.h"
#include "Kismet/GameplayStatics.h"
#include "DroneProject/Pawn/DronePawn.h"


AReconMission::AReconMission()
{
  PrimaryActorTick.bCanEverTick = false;
}

void AReconMission::BeginPlay()
{
  Super::BeginPlay();

  for (AReconTarget* Target : Targets)
  {
    if (Target)
    {
      Target->DeactivateTarget();
      Target->OnTargetCaptured.AddDynamic(
          this,
          &AReconMission::HandleTargetCaptured
      );
    }
  }
}

void AReconMission::StartMission()
{
  
  if (MissionStartSound)
  {
    UGameplayStatics::PlaySound2D(this, MissionStartSound);
  }

  if (TeachSound3)
  {
    GetWorld()->GetTimerManager().SetTimer(
        TeachSound3Timer,
        this,
        &AReconMission::PlayTeachSound3,
        1.0f,
        false
    );
  }
  
  CurrentScanTime = 0.f;
  ScanProgress = 0.f;

  CurrentIndex = 0;

  // 💥 1. Гарантированная подготовка
  for (AReconTarget* Target : Targets)
  {
    if (Target)
    {
      Target->DeactivateTarget();
      Target->OnTargetCaptured.RemoveAll(this);
      Target->OnTargetCaptured.AddDynamic(
          this,
          &AReconMission::HandleTargetCaptured
      );
    }
  }

  // 💥 2. Проверка
  if (Targets.Num() == 0)
  {
    UE_LOG(LogTemp, Error, TEXT("ReconMission: Targets array EMPTY"));
    return;
  }

  // 💥 3. Активируем первый
  Targets[0]->ActivateTarget();

  UE_LOG(LogTemp, Warning, TEXT("First Recon Target Activated"));
}

void AReconMission::HandleTargetCaptured(AReconTarget* Target)
{

  CurrentScanTime = 0.f;
  ScanProgress = 0.f;
  
  Target->DeactivateTarget();

  CurrentIndex++;

  if (Targets.IsValidIndex(CurrentIndex))
  {
    Targets[CurrentIndex]->ActivateTarget();
  }
  else
  {
    OnMissionCompleted.Broadcast();
  }
}


void AReconMission::UpdateScanProgress(float DeltaTime, bool bHit)
{
  if (!Targets.IsValidIndex(CurrentIndex)) return;

  if (bHit)
  {
    CurrentScanTime += DeltaTime;
  }
  else
  {
    CurrentScanTime = 0.f;
  }

  ScanProgress = FMath::Clamp(CurrentScanTime / RequiredScanTime, 0.f, 1.f);
}


void AReconMission::SaveMissionStartFromDrone()
{
  ADronePawn* Drone = Cast<ADronePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
  if (!Drone) return;

  MissionStartLocation = Drone->GetActorLocation();
  MissionStartRotation = Drone->GetActorRotation();
}

void AReconMission::PlayTeachSound3()
{
  if (TeachSound3)
  {
    UGameplayStatics::PlaySound2D(
        this,
        TeachSound3,
        2.0f   // 🔥 такая же громкость
    );
  }
}