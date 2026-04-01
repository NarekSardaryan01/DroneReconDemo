#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArtilleryMission.generated.h"

class AArtilleryTarget;
class AAntiAirTurret;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArtilleryMissionCompleted);

UCLASS()
class DRONEPROJECT_API AArtilleryMission : public AActor
{
  GENERATED_BODY()

public:
  AArtilleryMission();
  


  UPROPERTY(BlueprintAssignable)
  FOnArtilleryMissionCompleted OnMissionCompleted;

  UFUNCTION(BlueprintCallable)
  void StartMission();

  UFUNCTION(BlueprintCallable)
  void ResetMission();

  void SaveMissionStartFromDrone();

  UPROPERTY(BlueprintReadOnly)
  FVector MissionStartLocation = FVector::ZeroVector;

  UPROPERTY(BlueprintReadOnly)
  FRotator MissionStartRotation = FRotator::ZeroRotator;

  UPROPERTY(BlueprintReadOnly, Category="Artillery")
  float CurrentHoldTime = 0.f;

  UPROPERTY(BlueprintReadOnly, Category="Artillery")
  float RequiredHoldTime = 3.f;

  UPROPERTY(BlueprintReadOnly, Category="Artillery")
  float HoldProgress = 0.f;

  void UpdateHoldProgress(float DeltaTime, bool bHit);
  


protected:
  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category="Artillery")
  TArray<AArtilleryTarget*> Targets;

  // ================= SOUND =================

  UPROPERTY(EditAnywhere, Category="Mission|Sound")
  USoundBase* MissionStartSound;

  UPROPERTY(EditAnywhere, Category="Mission|Sound")
  USoundBase* TeachSound4;

  FTimerHandle TeachSound4Timer;

  void PlayTeachSound4();

private:

  int32 RemainingTargets = 0;
  bool bMissionActive = false;

  void CollectTargets();

  UFUNCTION()
  void HandleTargetCompleted(AArtilleryTarget* Target);
};