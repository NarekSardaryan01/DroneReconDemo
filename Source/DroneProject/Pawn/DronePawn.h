#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DroneProject/Missions/Recons/ReconMission.h"
#include "DroneProject/Missions/Artillery/ArtilleryMission.h"
#include "DronePawn.generated.h"


class UBoxComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class USceneCaptureComponent2D;
class UNiagaraSystem;
class USoundBase;
class UWidgetComponent;
class UUserWidget;
class AMissionManager;
class AReconMission;
class AArtilleryTarget;


UENUM(BlueprintType)
enum class EDroneFlightMode : uint8
{
    Speed UMETA(DisplayName="Speed"),
    Relax UMETA(DisplayName="Relax"),
    Land  UMETA(DisplayName="Land")
};

UCLASS()
class DRONEPROJECT_API ADronePawn : public APawn
{
    GENERATED_BODY()

public:
    ADronePawn();
    virtual void Tick(float DeltaTime) override;

    void RespawnDrone(const FVector& NewLocation, const FRotator& NewRotation);

protected:
    virtual void BeginPlay() override;


    UFUNCTION()
    void OnDroneHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit);

public:
    // =========================================================================
    // Input
    // =========================================================================
    void SetMoveForward(float Value);
    void SetMoveRight (float Value);
    void SetMoveUp    (float Value);
    void SetYawInput  (float Value);
    void SetFPVLookUp(float Value);
    void SetFPVTurn  (float Value);
    void SetCameraLookYaw  (float Value);
    void SetCameraLookPitch(float Value);

    float CameraLookYawInput   = 0.f;
    float CameraLookPitchInput = 0.f;

    // =========================================================================
    // Camera | Mouse Look
    // =========================================================================
    UPROPERTY(EditAnywhere, Category="Camera|MouseLook")
    float MouseLookYawSpeed = 90.f;

    UPROPERTY(EditAnywhere, Category="Camera|MouseLook")
    float MouseLookPitchSpeed = 70.f;

    UPROPERTY(EditAnywhere, Category="Camera|MouseLook")
    float MaxMouseLookYaw = 95.f;

    UPROPERTY(EditAnywhere, Category="Camera|MouseLook")
    float MaxMouseLookPitch = 75.f;

    UPROPERTY(EditAnywhere, Category="Camera|MouseLook")
    float MouseLookReturnSpeed = 1.5f;

    FRotator CameraLookOffset = FRotator::ZeroRotator;


    

    // =========================================================================
    // Engine
    // =========================================================================
    UFUNCTION(BlueprintCallable)
    void SetStartEngine();

    UFUNCTION(BlueprintCallable)
    void OnEngineTakeoffFinished();

    

    void UnlockControl();

    // =========================================================================
    // Components
    // =========================================================================
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* BoxCollision;

    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere)
    USceneCaptureComponent2D* FPVCamera;

    UPROPERTY(EditAnywhere, Category="Mission")
    TSubclassOf<UUserWidget> MissionWidgetClass;

    UPROPERTY(VisibleAnywhere, Category="Mission")
    UWidgetComponent* MissionWidgetComponent;

    //////Missions///
    FTimerHandle MissionDelayTimer;

    void ShowMissionWidget();
    
    UPROPERTY(BlueprintReadOnly, Category="Mission")
    AMissionManager* MissionManagerRef;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDroneCrashed);

    UPROPERTY(BlueprintAssignable)
    FOnDroneCrashed OnDroneCrashed;
    
    UFUNCTION()
    void OnTurretDestroyedShake();
    
    UPROPERTY(EditAnywhere, Category="Recon")
    AReconMission* ReconMission;

    FTimerHandle RespawnAfterDeathTimer;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDroneDestroyed);

    UPROPERTY(BlueprintAssignable)
    FOnDroneDestroyed OnDroneDestroyed;


    // =========================================================================
    // FPV Camera
    // =========================================================================
    UPROPERTY(EditAnywhere, Category="FPV|Camera")
    float FPVPitchSpeed = 80.f;

    UPROPERTY(EditAnywhere, Category="FPV|Camera")
    float FPVYawSpeed = 90.f;

    UPROPERTY(EditAnywhere, Category="FPV|Camera")
    float MinFPVPitch = -80.f;

    UPROPERTY(EditAnywhere, Category="FPV|Camera")
    float MaxFPVPitch = 35.f;

    UPROPERTY(EditAnywhere, Category="FPV|Camera")
    float MinFPVYaw = -95.f;

    UPROPERTY(EditAnywhere, Category="FPV|Camera")
    float MaxFPVYaw = 95.f;

    // =========================================================================
    // Movement
    // =========================================================================
    FVector MoveInput = FVector::ZeroVector;
    float   YawInput  = 0.f;
    FVector CurrentVelocity = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category="Movement")
    float MaxSpeed = 6500.f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float Acceleration = 0.8f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float Deceleration = 0.2f;

    // =========================================================================
    // Yaw
    // =========================================================================
    UPROPERTY(EditAnywhere, Category="Movement|Yaw")
    float YawSpeed = 130.f;

    UPROPERTY(EditAnywhere, Category="Movement|Yaw")
    float YawAcceleration = 4.5f;

    UPROPERTY(EditAnywhere, Category="Movement|Yaw")
    float YawDeceleration = 3.5f;

    float CurrentYawSpeed = 0.f;

    // =========================================================================
    // Control Lock
    // =========================================================================
    bool bControlLocked = true;

    UPROPERTY(EditAnywhere, Category="Movement|Stun")
    float ControlLockTime = 0.2f;

    FTimerHandle ControlLockTimer;

    // =========================================================================
    // Visual | Drone Tilt
    // =========================================================================
    void UpdateDroneTilt(float DeltaTime);

    UPROPERTY(EditAnywhere, Category="Movement|Tilt")
    float MaxTiltAngle = 28.f;

    UPROPERTY(EditAnywhere, Category="Movement|Tilt")
    float TiltInterpSpeed = 35.f;

    // =========================================================================
    // Engine State
    // =========================================================================
    UPROPERTY(BlueprintReadOnly)
    bool bEngineStarting = false;

    UPROPERTY(BlueprintReadOnly)
    bool bEngineStarted = false;

    // =========================================================================
    // Altitude
    // =========================================================================
    void UpdateAltitude();

    UPROPERTY(BlueprintReadOnly, Category="Altitude")
    float AltitudeMeters = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="Altitude")
    float AltitudeNormalized = 0.f;

    UPROPERTY(EditAnywhere, Category="Altitude")
    float MaxAltitudeMeters = 140.f;

    UPROPERTY(EditAnywhere, Category="Altitude")
    float TraceLength = 100000.f;

    // =========================================================================
    // FPV
    // =========================================================================
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="FPV")
    bool bFPVFullscreen = false;

    UFUNCTION(BlueprintCallable)
    void SetToggleFPV();

    void CaptureFPV();
    FTimerHandle FPVCaptureTimer;

    UPROPERTY(EditAnywhere, Category="FPV|Visual")
    float FPVShakeStrength = 0.35f;

    UPROPERTY(EditAnywhere, Category="FPV|Visual")
    float FPVShakeSpeed = 18.f;

    float FPVShakeTime = 0.f;
    
    FRotator FPVInputRotation = FRotator::ZeroRotator;
    FRotator SmoothedFPVRotation = FRotator::ZeroRotator;

    // =========================================================================
    // Flight Mode
    // =========================================================================
    UFUNCTION(BlueprintCallable, Category="Flight")
    void SetCycleFlightMode();

    UPROPERTY(BlueprintReadOnly, Category="Flight")
    EDroneFlightMode FlightMode = EDroneFlightMode::Relax;

    UPROPERTY(EditAnywhere, Category="Flight|Relax")
    float RelaxSpeedMultiplier = 0.48f;

    UPROPERTY(EditAnywhere, Category="Flight|Relax")
    float RelaxYawMultiplier = 0.75f;

    UPROPERTY(EditAnywhere, Category="Flight|Relax")
    float RelaxTiltMultiplier = 0.95f;

    UPROPERTY(EditAnywhere, Category="Landing")
    float LandSpeedMultiplier = 0.04f;

    UPROPERTY(EditAnywhere, Category="Landing")
    float LandYawMultiplier = 0.35f;

    UPROPERTY(EditAnywhere, Category="Landing")
    float LandTiltMultiplier = 0.3f;

    UPROPERTY(EditAnywhere, Category="Landing")
    float LandVerticalBoost = 1.2f;

    UPROPERTY(EditAnywhere, Category="Flight|Speed")
    float SpeedModeMultiplier = 1.0f;

    // =========================================================================
    // Main Camera | Tilt
    // =========================================================================
    void UpdateMainCameraTilt(float DeltaTime);

    UPROPERTY(EditAnywhere, Category="Camera|Tilt")
    float CameraTiltStrength = 0.40f;

    UPROPERTY(EditAnywhere, Category="Camera|Tilt")
    float CameraTiltInterpSpeed = 6.f;

    FRotator CameraTiltRotation = FRotator::ZeroRotator;

    // =========================================================================
    // Battery
    // =========================================================================
    void UpdateBattery(float DeltaTime);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battery")
    float BatteryPercent = 100.f;

    UPROPERTY(EditAnywhere, Category="Battery")
    float BatteryLifeSeconds = 300.f;

    UPROPERTY(EditAnywhere, Category="Battery")
    float SpeedModeDrainMultiplier = 1.3f;

    UPROPERTY(EditAnywhere, Category="Battery")
    float LandingDrainMultiplier = 0.05f;

    UPROPERTY(EditAnywhere, Category="Battery")
    float LowBatteryPercent = 5.f;

    // =========================================================================
    // Speed
    // =========================================================================
    UPROPERTY(BlueprintReadOnly, Category="Speed")
    int32 SpeedKmh = 0;

    float SmoothedSpeedKmh = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
    float MaxSpeedKmhUI = 92.f;

    UPROPERTY(BlueprintReadOnly, Category="Speed")
    float SpeedNormalized = 0.f;

    // =========================================================================
    // Wind (Drone)
    // =========================================================================
    void UpdateWindSway(float DeltaTime);

    UPROPERTY(EditAnywhere, Category="Wind")
    float WindJitterAngle = 0.30f;

    UPROPERTY(EditAnywhere, Category="Wind")
    float WindJitterFrequency = 20.f;

    UPROPERTY(EditAnywhere, Category="Wind")
    float WindSwayAngle = 3.0f;

    UPROPERTY(EditAnywhere, Category="Wind")
    float WindSwayFrequency = 1.0f;

    UPROPERTY(EditAnywhere, Category="Wind")
    float WindSwaySpeedStart = 1200.f;

    UPROPERTY(EditAnywhere, Category="Wind")
    float WindSwaySpeedMax = 5200.f;

    float WindSwayTime = 0.f;
    FRotator WindSwayRotation = FRotator::ZeroRotator;

    // =========================================================================
    // Main Camera | Speed Shake (NOT FPV)
    // =========================================================================
    void UpdateMainCameraShake(float DeltaTime);

    UPROPERTY(EditAnywhere, Category="Camera|Shake")
    float CameraShakeAngle = 1.8f;

    UPROPERTY(EditAnywhere, Category="Camera|Shake")
    float CameraShakeSpeed = 16.f;

    UPROPERTY(EditAnywhere, Category="Camera|Shake")
    float CameraShakeSpeedStart = 900.f;

    UPROPERTY(EditAnywhere, Category="Camera|Shake")
    float CameraShakeSpeedMax = 5200.f;

    float CameraShakeTime = 0.f;
    FRotator CameraShakeRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, Category="Camera|Shake")
    float CameraShakeLocation = 1.8f;

    FVector CameraShakeOffset = FVector::ZeroVector;
    FVector SpringArmBaseLocation = FVector::ZeroVector;

    // ===== AIM CAMERA OFFSET =====
    UPROPERTY(EditAnywhere, Category="Camera|Aim")
    float AimCameraSideOffset = 55.f;

    UPROPERTY(EditAnywhere, Category="Camera|Aim")
    float AimCameraInterpSpeed =3.0f;

    FVector CurrentAimOffset = FVector::ZeroVector;
    FRotator SpringArmBaseRotation = FRotator::ZeroRotator;

    // ==================== SOUND ====================
    
    UPROPERTY(VisibleAnywhere, Category="Sound")
    UAudioComponent* TakeoffAudio;

    UPROPERTY(VisibleAnywhere, Category="Sound")
    UAudioComponent* HoverAudio;

    UPROPERTY(VisibleAnywhere, Category="Sound")
    UAudioComponent* BoostAudio;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* TakeoffSound;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* HoverSound;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* BoostSound;


    UPROPERTY(VisibleAnywhere, Category="Sound")
    UAudioComponent* WarningAudio;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* WarningSound;

    UPROPERTY(EditAnywhere, Category="Sound")
    float WarningMaxVolume = 0.9f;

    UPROPERTY(EditAnywhere, Category="Sound")
    float WarningFadeSpeed = 4.f;

    // ================= COLLISION SOUND =================

    UPROPERTY(EditAnywhere, Category="Sound|Collision")
    USoundBase* CollisionSound;

    UPROPERTY(VisibleAnywhere, Category="Sound|Collision")
    UAudioComponent* CollisionAudio;

    UPROPERTY(EditAnywhere, Category="Sound|Collision")
    float CollisionMinVolume = 0.25f;

    UPROPERTY(EditAnywhere, Category="Sound|Collision")
    float CollisionMaxVolume = 1.0f;

    UPROPERTY(EditAnywhere, Category="Sound|Collision")
    float CollisionMinBounce = 350.f;  // Relax
     
    UPROPERTY(EditAnywhere, Category="Sound|Collision")
    float CollisionMaxBounce = 750.f;  // Speed

    UPROPERTY(EditAnywhere, Category="Sound|HitWarning")
    USoundBase* HitWarningSound;

    UPROPERTY(VisibleAnywhere, Category="Sound|HitWarning")
    UAudioComponent* HitWarningAudio;

    FTimerHandle HitWarningTimer;

    
    UFUNCTION()
    void StopHitWarningSound();
    

    UPROPERTY(EditAnywhere, Category="Sound")
    float HoverVolume = 0.85f;

    UPROPERTY(EditAnywhere, Category="Sound")
    float BoostMinVolume = 0.0f;

    UPROPERTY(EditAnywhere, Category="Sound")
    float BoostMaxVolumeRelax = 0.5f;

    UPROPERTY(EditAnywhere, Category="Sound")
    float BoostMaxVolumeSpeed =0.8;

    void StartHoverSound();

    FTimerHandle TakeoffToHoverTimer;

    float BoostIntent = 0.f; 

    UPROPERTY(EditAnywhere, Category="Sound")
    float BoostFadeInSpeed = 6.f;

    UPROPERTY(EditAnywhere, Category="Sound")
    float BoostFadeOutSpeed = 3.f;


    UPROPERTY(EditAnywhere, Category="Sound")
    float BoostOnThreshold = 0.04f;

    UPROPERTY(EditAnywhere, Category="Sound")
    float BoostOffThreshold = 0.015f;

    UPROPERTY(EditAnywhere, Category="Sound")
    float BoostIdleVolume =  0.4f;  

    UPROPERTY(EditAnywhere, Category="Sound")
    float BoostHoverMaxVolume = 0.4f;


    // ==================== ANTI-DRONE ====================

    UPROPERTY(BlueprintReadOnly, Category="AntiDrone")
    float InterferenceLevel = 0.f;

    UPROPERTY(EditAnywhere, Category="AntiDrone")
    float InterferenceRiseSpeed = 0.25f;

    UPROPERTY(EditAnywhere, Category="AntiDrone")
    float InterferenceFallSpeed = 0.15f;

    bool bInsideAntiDroneZone = false;


    float BaseFPVShakeStrength;
    float BaseWindJitterAngle;
    float BaseWindSwayAngle;
    float BaseCameraShakeAngle;
    float BaseCameraShakeLocation;

    float BaseAcceleration;
    float BaseYawAcceleration;

    bool bAntiDroneEverTriggered = false;


    // ==================== FIX ZONE ====================

    bool bInsideFixZone = false;

    UPROPERTY(EditAnywhere, Category="Fix")
    float FixSpeed = 0.8f;

    UPROPERTY(EditAnywhere, Category="Fix")
    float HealthHealSpeed = 20.f;

    void StartFixing();
    void StopFixing();
    bool bFixedOnce = false;

    // ==================== UNDER FIRE ====================

    UPROPERTY(BlueprintReadOnly, Category="Combat")
    bool bUnderTurretFire = false;

    UFUNCTION(BlueprintCallable)
    void SetUnderTurretFire(bool bState);

    // ==================== TURRET WARNING ====================

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* TurretWarningSound;

    UPROPERTY(VisibleAnywhere, Category="Sound")
    UAudioComponent* TurretWarningAudio;

    // ================= HEALTH =================

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
    float CurrentHealth;

    UFUNCTION()
    void ApplyDamage(float DamageAmount);


    // ================= EXPLOSION =================

    UPROPERTY(EditDefaultsOnly, Category="Explosion")
    UNiagaraSystem* ExplosionNiagara;

    UPROPERTY(EditDefaultsOnly, Category="Explosion")
    USoundBase* ExplosionSound;

    UPROPERTY(VisibleAnywhere, Category="Explosion")
    UAudioComponent* ExplosionAudio;

    UPROPERTY(EditAnywhere, Category="Explosion")
    FVector ExplosionScale = FVector(2.f);




    bool bDestroyed = false;

    void Explode();


    // ===== Camera Explosion Shake =====
    UPROPERTY(EditAnywhere, Category="Explosion|Camera")
    float ExplosionCameraAngle = 23.f;

    UPROPERTY(EditAnywhere, Category="Explosion|Camera")
    float ExplosionCameraLocation = 60.f;

    UPROPERTY(EditAnywhere, Category="Explosion|Camera")
    float ExplosionCameraDuration = 0.45f;

    float ExplosionCameraTime = 0.f;
    bool bExplosionCameraActive = false;



     ////missions Widget////
    UFUNCTION()
    void HandleTutorialFinished();


    // ================= RECON =================

    UPROPERTY(BlueprintReadOnly, Category="Recon")
    bool bReconScanning = false;


    UFUNCTION()
    void StartReconScan();

    UFUNCTION()
    void StopReconScan();

    
    UPROPERTY(EditAnywhere, Category="Recon")
    float ReconTraceDistance = 15000.f;

    // ================= OVERLAY BEAM SETTINGS =================

    UPROPERTY(EditAnywhere, Category="Recon|Overlay")
    float ReconOverlayLength = 15000.f;

    UPROPERTY(EditAnywhere, Category="Recon|Overlay")
    FVector ReconOverlayScale = FVector(1.f, 1.f, 1.f);

    UPROPERTY(EditAnywhere, Category="Recon")
    float ReconTraceThickness = 2.5f;

    void PerformReconTrace(float DeltaTime);

    // ================= ARTILLERY DESIGNATION =================

    UPROPERTY(BlueprintReadOnly, Category="Artillery")
    bool bArtilleryDesignating = false;

    UPROPERTY(EditAnywhere, Category="Artillery")
    float ArtilleryTraceDistance = 15000.f;

    UPROPERTY(EditAnywhere, Category="Artillery|Overlay")
    float ArtilleryOverlayLength = 15000.f;

    UPROPERTY(EditAnywhere, Category="Artillery|Overlay")
    FVector ArtilleryOverlayScale = FVector(1.f, 1.f, 1.f);

    UPROPERTY()
    AArtilleryTarget* CurrentArtilleryTarget = nullptr;

    UFUNCTION(BlueprintCallable)
    void SetStartArtilleryDesignate();

    UFUNCTION(BlueprintCallable)
    void SetStopArtilleryDesignate();

    void PerformArtilleryTrace(float DeltaTime);


    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* ReconBeam;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* ArtilleryBeam;

    UPROPERTY(VisibleAnywhere)
    USceneComponent* ReconBeamRoot;

    UPROPERTY(VisibleAnywhere)
    USceneComponent* ArtilleryBeamRoot;

    // RECON
    UPROPERTY(VisibleAnywhere)
    USceneComponent* ReconBeamOverlayRoot;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* ReconBeamOverlay;


    // ARTILLERY
    UPROPERTY(VisibleAnywhere)
    USceneComponent* ArtilleryBeamOverlayRoot;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* ArtilleryBeamOverlay;

 


     //================= TARGET MARKER =================


    UPROPERTY(VisibleAnywhere, Category="Marker")
    UStaticMeshComponent* LaserMarker;


    UPROPERTY(EditDefaultsOnly, Category="Marker")
    UMaterialInterface* MI_MarkerRed;

    UPROPERTY(EditDefaultsOnly, Category="Marker")
    UMaterialInterface* MI_MarkerBlue;


    // ================= MARKER DISTANCE SCALE =================

    UPROPERTY(EditAnywhere, Category="Marker|DistanceScale")
    float MarkerMaxTraceDistance = 15000.f; 

    UPROPERTY(EditAnywhere, Category="Marker|DistanceScale")
    float MarkerScaleNear = 1.0f;            

    UPROPERTY(EditAnywhere, Category="Marker|DistanceScale")
    float MarkerScaleFar = 8.0f;    

    float MarkerSpinAngle = 0.f;
    float MarkerPulseTime = 0.f;

    void UpdateLaserMarker(const FHitResult& Hit,bool bIsRecon,bool bIsArtillery,float HoldProgress
);


    // ================= LASER SOUND =================

    UPROPERTY(EditAnywhere, Category="Sound|Laser")
    USoundBase* LaserIdleSound;
    
    UPROPERTY(VisibleAnywhere, Category="Sound|Laser")
    UAudioComponent* LaserAudio;
    

    void UpdateLaserSound(bool bHitTarget);

    UPROPERTY(EditAnywhere, Category="Sound|Laser")
    USoundBase* LaserLockSound;

    bool bLaserLocked = false;
};