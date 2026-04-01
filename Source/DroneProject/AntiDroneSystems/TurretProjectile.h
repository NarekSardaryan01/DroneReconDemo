#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class USoundBase;
class ADronePawn;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class DRONEPROJECT_API ATurretProjectile : public AActor
{
  GENERATED_BODY()

public:
  ATurretProjectile();

  virtual void BeginPlay() override;

  // Target (set by turret)
protected:

  UPROPERTY(VisibleAnywhere)
  USphereComponent* Collision;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* Mesh;

  UPROPERTY(VisibleAnywhere)
  UProjectileMovementComponent* Movement;

  UPROPERTY(EditDefaultsOnly, Category="Effects")
  USoundBase* ImpactSound;

  UPROPERTY(EditAnywhere, Category="Effects")
  float ImpactSoundVolume = 0.5f;

  UPROPERTY(EditDefaultsOnly, Category="Effects")
  UNiagaraSystem* ImpactNiagara;


  // Air Effect
  UPROPERTY(EditDefaultsOnly, Category="Effects")
  UNiagaraSystem* TrailNiagara;

  UPROPERTY(VisibleAnywhere)
  UNiagaraComponent* TrailComponent;


  // Impact Niagara
  UPROPERTY(EditAnywhere, Category="Effects")
  FVector ImpactNiagaraScale = FVector(1.f);

  // Trail Niagara
  UPROPERTY(EditAnywhere, Category="Effects")
  FVector TrailNiagaraScale = FVector(1.f);

//Demage////////
  UPROPERTY(EditAnywhere, Category="Damage")
  float Damage = 0.4f;

  UFUNCTION()
  void OnHit(
      UPrimitiveComponent* HitComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      FVector NormalImpulse,
      const FHitResult& Hit
  );
};