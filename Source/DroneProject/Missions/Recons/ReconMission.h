#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReconMission.generated.h"

class AReconTarget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReconMissionCompleted);

UCLASS()
class DRONEPROJECT_API AReconMission : public AActor
{
  GENERATED_BODY()

public:
  AReconMission();

  UPROPERTY(BlueprintAssignable)
  FOnReconMissionCompleted OnMissionCompleted;

  void StartMission();

  // 👉 Для Widget
  UPROPERTY(BlueprintReadOnly, Category="Recon")
  float CurrentScanTime = 0.f;

  UPROPERTY(BlueprintReadOnly, Category="Recon")
  float RequiredScanTime = 3.f;

  UPROPERTY(BlueprintReadOnly, Category="Recon")
  float ScanProgress = 0.f;

  void UpdateScanProgress(float DeltaTime, bool bHit);
  
  UPROPERTY(BlueprintReadOnly, Category="Recon")
  FVector MissionStartLocation = FVector::ZeroVector;

  UPROPERTY(BlueprintReadOnly, Category="Recon")
  FRotator MissionStartRotation = FRotator::ZeroRotator;

  UFUNCTION(BlueprintCallable, Category="Recon")
  void SaveMissionStartFromDrone();

  UPROPERTY(EditAnywhere, Category="Mission|Sound")
  USoundBase* MissionStartSound;

  UPROPERTY(EditAnywhere, Category="Mission|Sound")
  USoundBase* TeachSound3;

  FTimerHandle TeachSound3Timer;

  void PlayTeachSound3();

protected:
  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category="Recon")
  TArray<AReconTarget*> Targets;

private:
  int32 CurrentIndex = 0;

  UFUNCTION()
  void HandleTargetCaptured(AReconTarget* Target);



};