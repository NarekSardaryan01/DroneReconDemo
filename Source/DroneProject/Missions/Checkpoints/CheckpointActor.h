#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckpointActor.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCheckpointPassed, ACheckpointActor*, Checkpoint);

UCLASS()
class DRONEPROJECT_API ACheckpointActor : public AActor
{
  GENERATED_BODY()

public:
  ACheckpointActor();

  UPROPERTY(BlueprintAssignable)
  FOnCheckpointPassed OnCheckpointPassed;

  void ActivateCheckpoint();
  void DeactivateCheckpoint();

protected:
  virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere)
  USphereComponent* Sphere;

  UPROPERTY(VisibleAnywhere)
  USkeletalMeshComponent* VisualMesh;

  UPROPERTY(EditAnywhere, Category="Checkpoint|Sound")
  USoundBase* CheckpointPassSound;

  UPROPERTY(EditAnywhere, Category="Checkpoint|Sound")
  float CheckpointVolumeMultiplier = 2.0f; 

  bool bIsActive = false;

  UFUNCTION()
  void OnSphereOverlap(
      UPrimitiveComponent* OverlappedComp,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult& SweepResult
  );
};