#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AChargingZone.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class ADronePawn;

UCLASS()
class DRONEPROJECT_API AChargingZone : public AActor
{
  GENERATED_BODY()

public:
  AChargingZone();

  virtual void Tick(float DeltaTime) override;

protected:
  virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere)
  UBoxComponent* Trigger;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* Visual;

  UPROPERTY(EditAnywhere, Category="Charging")
  float ChargePerSecond =10.f;

  UFUNCTION()
  void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResul);

  UFUNCTION()
  void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);

  TWeakObjectPtr<ADronePawn> ChargingDrone;
  
};