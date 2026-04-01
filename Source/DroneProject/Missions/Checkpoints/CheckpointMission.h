#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckpointMission.generated.h"

class ACheckpointActor;
class ADronePawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionCompleted);

UCLASS()
class DRONEPROJECT_API ACheckpointMission : public AActor
{
  GENERATED_BODY()

public:
  ACheckpointMission();

  UPROPERTY(BlueprintAssignable)
  FOnMissionCompleted OnMissionCompleted;

  UFUNCTION(BlueprintCallable)
  void StartMission();


  UPROPERTY(BlueprintReadOnly, Category="Mission")
  float RemainingTime = 0.f;

  void FinishMission();

  FTimerHandle CrashDelayHandle;

  UFUNCTION()
  void ResetMission();

  UPROPERTY(EditAnywhere, Category="Mission|Sound")
  USoundBase* MissionStartSound;

protected:
  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category="Mission")
  TArray<ACheckpointActor*> Checkpoints;

  UPROPERTY(EditAnywhere, Category="Mission")
  float MissionDuration = 50.f;

private:
  FTimerHandle MissionTimerHandle;
  FTimerHandle CountdownTimerHandle;

  int32 CurrentCheckpointIndex = 0;

  UPROPERTY()
  ADronePawn* CachedDrone = nullptr;

  FVector SavedDroneLocation = FVector::ZeroVector;
  FRotator SavedDroneRotation = FRotator::ZeroRotator;

  void HandleTimeExpired();

  void UpdateCountdown();

  UFUNCTION()
  void OnCheckpointPassed(ACheckpointActor* PassedCheckpoint);

  UFUNCTION()
  void HandleDroneCrash();
};