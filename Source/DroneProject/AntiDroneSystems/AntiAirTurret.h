#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretProjectile.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "AntiAirTurret.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class ADronePawn;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class DRONEPROJECT_API AAntiAirTurret : public AActor
{
  GENERATED_BODY()

public:
  AAntiAirTurret();
  virtual void Tick(float DeltaTime) override;

  DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurretDestroyedByArtillery);

  UPROPERTY(BlueprintAssignable)
  FOnTurretDestroyedByArtillery OnTurretDestroyedByArtillery;

  // ================= DESTROY (Artillery) =================
  UFUNCTION(BlueprintCallable, Category="Turret")
  void DestroyByArtilleryStrike();


  UPROPERTY(EditDefaultsOnly, Category="Explosion")
  UNiagaraSystem* ExplosionNiagara;

  UPROPERTY(EditDefaultsOnly, Category="Explosion")
  USoundBase* ExplosionSound;

  UPROPERTY(EditAnywhere, Category="Explosion")
  FVector ExplosionScale = FVector(2.f);

  UFUNCTION(BlueprintCallable)
  void ResetTurret();


  // ================= DESTROY VISUAL =================

  UPROPERTY(EditAnywhere, Category="Explosion")
  UMaterialInterface* DestroyedMaterial = nullptr;
  
  UPROPERTY()
  TArray<UMaterialInterface*> OriginalMaterials;
  
  
protected:
  virtual void BeginPlay() override;



  
  // ================= COMPONENTS =================

  UPROPERTY(VisibleAnywhere)
  USceneComponent* Root;

  UPROPERTY(VisibleAnywhere)
  USkeletalMeshComponent* TurretMesh;

  UPROPERTY(VisibleAnywhere)
  USphereComponent* DetectionSphere;

  // ================= TARGET =================

  UPROPERTY()
  ADronePawn* TargetDrone;

  // ================= SETTINGS =================

  UPROPERTY(EditAnywhere, Category="Turret")
  float DetectionRadius = 4000.f;

  UPROPERTY(EditAnywhere, Category="Turret")
  float RotationInterpSpeed = 35.f;

  UPROPERTY(EditAnywhere, Category="Turret")
  float MinPitch = -10.f;

  UPROPERTY(EditAnywhere, Category="Turret")
  float MaxPitch = 65.f;

  UPROPERTY(EditAnywhere, Category="Turret")
  float MinDistanceXY = 150.f;

  // ================= VFX =================

  UPROPERTY(EditDefaultsOnly, Category="VFX")
  UNiagaraSystem* MuzzleFlashSystem;

  UPROPERTY(VisibleAnywhere)
  UNiagaraComponent* MuzzleFlashComponent;


  UPROPERTY(EditAnywhere, Category="VFX")
  float MuzzleFlashScale = 1.0f;


  // ================= SOUND =================

  UPROPERTY(VisibleAnywhere)
  UAudioComponent* FireAudio;

  UPROPERTY(EditAnywhere, Category="Sound")
  USoundBase* FireLoopSound;

  
  // ================= INTERNAL =================

  float CurrentYaw = 0.f;
  float CurrentPitch = 0.f;

  // ================= OVERLAP =================

  UFUNCTION()
  void OnEnterSphere(
      UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult& SweepResult
  );

  UFUNCTION()
  void OnExitSphere(
      UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex
  );

  UPROPERTY(EditAnywhere, Category="Fire")
  TSubclassOf<ATurretProjectile> ProjectileClass;

  UPROPERTY(EditAnywhere, Category="Fire")
  float FireInterval = 0.03;

  FTimerHandle FireTimer;

  void StartFiring();
  void StopFiring();
  void Fire();

  // ================= DESTROY INTERNAL =================
  bool bDestroyed = false;

  UPROPERTY(EditAnywhere, Category="Turret|Destroy")
  UNiagaraSystem* DestroyFX = nullptr;

  UPROPERTY(EditAnywhere, Category="Turret|Destroy")
  USoundBase* DestroySound = nullptr;

  UPROPERTY(EditAnywhere, Category="Turret|Destroy")
  float DestroyLifeSpan = 2.5f;

  void DestroyTurret_Internal();


};