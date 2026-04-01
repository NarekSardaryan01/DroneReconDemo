#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "ArtilleryShell.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UStaticMeshComponent;
class AAntiAirTurret;

UCLASS()
class DRONEPROJECT_API AArtilleryShell : public AActor
{
  GENERATED_BODY()

public:
  AArtilleryShell();

  virtual void Tick(float DeltaTime) override;

  UPROPERTY()
  AAntiAirTurret* TargetTurret = nullptr;

  UPROPERTY()
  FVector ImpactPoint = FVector::ZeroVector;

  void SetImpactPoint(const FVector& InPoint);

public:
  
  void SetTargetTurret(AAntiAirTurret* InTurret);

protected:
  virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere)
  USceneComponent* Root;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* Mesh;

  UPROPERTY(VisibleAnywhere)
  USphereComponent* Collision;

  UPROPERTY(EditAnywhere, Category="Explosion")
  UNiagaraSystem* ExplosionFX;

  UPROPERTY(EditAnywhere, Category="Explosion")
  USoundBase* ExplosionSound;

  UPROPERTY(EditAnywhere, Category="Movement")
  float FallSpeed = 18000.f;

  UPROPERTY(VisibleAnywhere, Category="Audio")
  UAudioComponent* FallAudioComponent;

  UPROPERTY(EditAnywhere, Category="Audio")
  USoundBase* FallingSound;

  UFUNCTION()
  void OnHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit);
};