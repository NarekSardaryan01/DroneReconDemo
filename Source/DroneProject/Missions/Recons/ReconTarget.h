#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReconTarget.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReconTargetCaptured, AReconTarget*, Target);

UCLASS()
class DRONEPROJECT_API AReconTarget : public AActor
{
  GENERATED_BODY()

public:
  AReconTarget();

  // Делегат — сообщает миссии что цель снята
  UPROPERTY(BlueprintAssignable)
  FOnReconTargetCaptured OnTargetCaptured;

  void ActivateTarget();
  void DeactivateTarget();

  void ProcessScan(float DeltaTime, bool bHitByScan);

protected:
  virtual void BeginPlay() override;

  // Маленький хитбокс для фото
  UPROPERTY(VisibleAnywhere)
  UBoxComponent* PhotoCollision;

  // Визуальный цилиндр
  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* VisualCylinder;

  // Время сканирования
  UPROPERTY(EditAnywhere, Category="Recon")
  float RequiredScanTime = 3.f;

  // 🔊 Snapshot sound
  UPROPERTY(EditAnywhere, Category="Recon|Sound")
  USoundBase* SnapshotSound = nullptr;

  UPROPERTY(EditAnywhere, Category="Recon|Sound")
  float SnapshotVolume = 1.0f;

private:
  float CurrentScanTime = 0.f;
  bool bActive = false;
};