#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFixDroneZone.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class DRONEPROJECT_API AFixDroneZone : public AActor
{
  GENERATED_BODY()

public:
  AFixDroneZone();

protected:
  UPROPERTY(VisibleAnywhere)
  UBoxComponent* Trigger;
  
  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* VisualMesh;

  UFUNCTION()
  void OnEnter(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult
  );

  UFUNCTION()
  void OnExit(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
};