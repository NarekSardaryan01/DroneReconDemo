#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FinalTarget.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFinalTargetTriggered, AFinalTarget*, Target);

UCLASS()
class DRONEPROJECT_API AFinalTarget : public AActor
{
  GENERATED_BODY()

public:
  AFinalTarget();

  UPROPERTY(BlueprintAssignable)
  FOnFinalTargetTriggered OnTargetTriggered;

  UFUNCTION(BlueprintCallable)
  void ActivateTarget();

  UFUNCTION(BlueprintCallable)
  void DeactivateTarget();

protected:
  virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere)
  UBoxComponent* FinishCollision;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* VisualCylinder;

  UFUNCTION()
  void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

private:
  bool bActive = false;
  bool bTriggered = false;
};