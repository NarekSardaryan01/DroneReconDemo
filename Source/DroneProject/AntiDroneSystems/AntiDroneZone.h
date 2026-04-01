#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AntiDroneZone.generated.h"

class UBoxComponent;

UCLASS()
class DRONEPROJECT_API AAntiDroneZone : public AActor
{
  GENERATED_BODY()

public:
  AAntiDroneZone();

protected:
  UPROPERTY(VisibleAnywhere)
  UBoxComponent* Trigger;

  
  UFUNCTION()
  void OnEnter(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResul);

  UFUNCTION()
  void OnExit(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);

  
};