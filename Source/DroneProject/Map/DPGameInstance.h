#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DPGameInstance.generated.h"

class USoundBase;
class UAudioComponent;

UCLASS()
class DRONEPROJECT_API UDPGameInstance : public UGameInstance
{
  GENERATED_BODY()

public:
  virtual void OnStart() override;


  void SetDroneSpeed01(float Speed01);

protected:
  // ==================== AMBIENT ====================
  UPROPERTY(EditAnywhere, Category="Audio")
  USoundBase* AmbientSound;

  UPROPERTY()
  UAudioComponent* AmbientAudio;

  // ==================== TEACH ====================

  UPROPERTY(EditAnywhere, Category="Audio|Tutorial")
  USoundBase* TeachSound1;

  FTimerHandle TeachSoundTimer;

  // ==================== WIND ====================
  UPROPERTY(EditAnywhere, Category="Audio|Wind")
  USoundBase* WindSound;

  UPROPERTY(Transient)
  UAudioComponent* WindAudio;


  float DroneSpeed01 = 0.f;

  // ==================== WIND TUNING ====================
  UPROPERTY(EditAnywhere, Category="Audio|Wind")
  float WindStartThreshold = 0.05f;
  
  UPROPERTY(EditAnywhere, Category="Audio|Wind")
  float WindMaxVolume = 0.25f;

  UPROPERTY(EditAnywhere, Category="Audio|Wind")
  float WindMinPitch = 0.95f;

  UPROPERTY(EditAnywhere, Category="Audio|Wind")
  float WindMaxPitch = 1.12f;
};