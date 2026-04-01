#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FinalMission.generated.h"

class AFinalTarget;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinalMissionCompleted);

UCLASS()
class DRONEPROJECT_API AFinalMission : public AActor
{
  GENERATED_BODY()

public:
  AFinalMission();

  virtual void Tick(float DeltaTime) override;

  UPROPERTY(BlueprintAssignable)
  FOnFinalMissionCompleted OnMissionCompleted;

  UFUNCTION(BlueprintCallable)
  void StartMission();

  UFUNCTION(BlueprintCallable)
  void ResetMission();



  UPROPERTY(BlueprintReadOnly, Category="Final")
  float CurrentHoldTime = 0.f;

  UPROPERTY(BlueprintReadOnly, Category="Final")
  float RequiredHoldTime = 5.f;

  UPROPERTY(BlueprintReadOnly, Category="Final")
  float HoldProgress = 0.f;

  UFUNCTION(BlueprintPure, Category="Final")
  float GetRemainingTime() const { return FMath::Max(RequiredHoldTime - CurrentHoldTime, 0.f); }

protected:
  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category="Final")
  AFinalTarget* FinalTarget;

  UPROPERTY(EditAnywhere, Category="Mission|Sound")
  USoundBase* MissionStartSound;

  UPROPERTY(EditAnywhere, Category="Mission|Sound")
  USoundBase* TeachSound5;

  UPROPERTY(EditAnywhere, Category="Mission|Sound")
  float TeachSoundDelay = 1.0f;

  FTimerHandle TeachSoundTimer;

  void PlayTeachSound5();
  void CompleteGame();

private:
  UFUNCTION()
  void HandleTargetTriggered(AFinalTarget* Target);

  bool bMissionActive = false;
  bool bEndCountdownStarted = false;
};