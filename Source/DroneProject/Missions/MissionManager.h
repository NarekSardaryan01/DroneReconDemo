#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneProject/Missions/Recons/ReconMission.h"
#include "DroneProject/Missions/Artillery/ArtilleryMission.h"
#include "MissionManager.generated.h"

class ACheckpointMission;
class AReconMission;
class AArtilleryMission;
class AFinalMission;

UENUM(BlueprintType)
enum class EMissionType : uint8
{
  None        UMETA(DisplayName="None"),
  Checkpoint  UMETA(DisplayName="Checkpoint"),
  Recon       UMETA(DisplayName="Recon"),
  Artillery   UMETA(DisplayName="Artillery"),
  Final       UMETA(DisplayName="Final")
};

UCLASS()
class DRONEPROJECT_API AMissionManager : public AActor
{
  GENERATED_BODY()

public:
  AMissionManager();
  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
  ACheckpointMission* CheckpointMission;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
  AReconMission* ReconMission;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
  AArtilleryMission* ArtilleryMission;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
  AFinalMission* FinalMission;

  UPROPERTY(BlueprintReadOnly, Category="Mission")
  EMissionType CurrentMissionType = EMissionType::None;

  FTimerHandle StartFinalMissionTimer;

  UFUNCTION()
  void StartFinalMissionDelayed();

  UFUNCTION(BlueprintCallable)
  void StartCheckpointMission();

private:
  UFUNCTION()
  void OnCheckpointMissionCompleted();

  UFUNCTION()
  void OnReconMissionCompleted();

  UFUNCTION()
  void OnArtilleryMissionCompleted();

  UFUNCTION()
  void OnFinalMissionCompleted();

  UFUNCTION()
  void HandleDroneDestroyed();

  UFUNCTION()
  void RestartReconMission();

  UFUNCTION()
  void RestartArtilleryMission();

  UFUNCTION()
  void RestartFinalMission();

  FTimerHandle RestartActiveMissionTimer;
};