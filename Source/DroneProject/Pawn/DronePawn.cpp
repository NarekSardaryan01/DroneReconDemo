#include "DronePawn.h"
#include "DroneProject/Map/DPGameInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/WidgetComponent.h"
#include "DroneProject/Tutorial/ATutorialManager.h"
#include "EngineUtils.h"
#include "DroneProject/Missions/MissionManager.h"
#include "DroneProject/Missions/Recons/ReconTarget.h"
#include "DroneProject/Missions/Recons/ReconMission.h"
#include "DroneProject/Missions/Artillery/ArtilleryTarget.h"
#include "DroneProject/Missions/Artillery/ArtilleryMission.h"
#include "DroneProject/AntiDroneSystems/AntiAirTurret.h"
#include "NiagaraComponent.h"


// ============================================================================
// Constructor
// ============================================================================
ADronePawn::ADronePawn()
{
    PrimaryActorTick.bCanEverTick = true; 


    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    SetRootComponent(BoxCollision);

    BoxCollision->SetCollisionObjectType(ECC_Pawn);
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxCollision->SetCollisionResponseToAllChannels(ECR_Block);
    BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    BoxCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // ВАЖНО
    BoxCollision->SetGenerateOverlapEvents(true);
    BoxCollision->SetNotifyRigidBodyCollision(true);
    BoxCollision->OnComponentHit.AddDynamic(this,&ADronePawn::OnDroneHit);
    

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(BoxCollision);
    Mesh->SetCollisionObjectType(ECC_Pawn);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionResponseToAllChannels(ECR_Block);
    Mesh->SetSimulatePhysics(false);
    Mesh->SetGenerateOverlapEvents(false);




    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(BoxCollision);
    SpringArm->bDoCollisionTest = true;
    SpringArm->ProbeChannel = ECC_Camera;
    SpringArm->ProbeSize = 20.f;
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->SetUsingAbsoluteRotation(false);
    SpringArm->bEnableCameraLag = false;
    SpringArm->bEnableCameraRotationLag = true;
    SpringArm->CameraRotationLagSpeed = 12.f;

    
    TakeoffAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("TakeoffAudio"));
    TakeoffAudio->SetupAttachment(BoxCollision);
    TakeoffAudio->bAutoActivate = false;
    

    HoverAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("HoverAudio"));
    HoverAudio->SetupAttachment(BoxCollision);
    HoverAudio->bAutoActivate = false;
    

    BoostAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("BoostAudio"));
    BoostAudio->SetupAttachment(BoxCollision);
    BoostAudio->bAutoActivate = false;

    
    WarningAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("WarningAudio"));
    WarningAudio->SetupAttachment(BoxCollision);
    WarningAudio->bAutoActivate = false;
    

    TurretWarningAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("TurretWarningAudio"));
    TurretWarningAudio->SetupAttachment(BoxCollision);
    TurretWarningAudio->bAutoActivate = false;
    

    ExplosionAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ExplosionAudio"));
    ExplosionAudio->SetupAttachment(BoxCollision);
    ExplosionAudio->bAutoActivate = false;
    ExplosionAudio->bStopWhenOwnerDestroyed = false;

    CollisionAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("CollisionAudio"));
    CollisionAudio->SetupAttachment(BoxCollision);
    CollisionAudio->bAutoActivate = false;

    HitWarningAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("HitWarningAudio"));
    HitWarningAudio->SetupAttachment(BoxCollision);
    HitWarningAudio->bAutoActivate = false;

    LaserAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("LaserAudio"));
    LaserAudio->SetupAttachment(BoxCollision);
    LaserAudio->bAutoActivate = false;

    MissionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MissionWidget"));
    MissionWidgetComponent->SetupAttachment(BoxCollision);
    MissionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    MissionWidgetComponent->SetVisibility(false);
    
    
--
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

    FPVCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("FPVCamera"));
    FPVCamera->SetupAttachment(BoxCollision);


    ReconBeamRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ReconBeamRoot"));
    ReconBeamRoot->SetupAttachment(FPVCamera);


    ReconBeamRoot->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    ReconBeam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReconBeam"));
    ReconBeam->SetupAttachment(ReconBeamRoot);
    ReconBeam->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ReconBeam->SetCastShadow(false);
    ReconBeam->SetVisibility(false);


    ArtilleryBeamRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ArtilleryBeamRoot"));
    ArtilleryBeamRoot->SetupAttachment(FPVCamera);
    ArtilleryBeamRoot->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    ArtilleryBeam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArtilleryBeam"));
    ArtilleryBeam->SetupAttachment(ArtilleryBeamRoot);
    ArtilleryBeam->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ArtilleryBeam->SetCastShadow(false);
    ArtilleryBeam->SetVisibility(false);


    ReconBeamOverlayRoot =CreateDefaultSubobject<USceneComponent>(TEXT("ReconBeamOverlayRoot"));
    ReconBeamOverlayRoot->SetupAttachment(FPVCamera);
    ReconBeamOverlayRoot->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    ReconBeamOverlay =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReconBeamOverlay"));
    ReconBeamOverlay->SetupAttachment(ReconBeamOverlayRoot);
    ReconBeamOverlay->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ReconBeamOverlay->SetCastShadow(false);
    ReconBeamOverlay->SetVisibility(false);

    

    ArtilleryBeamOverlayRoot =CreateDefaultSubobject<USceneComponent>(TEXT("ArtilleryBeamOverlayRoot"));
    ArtilleryBeamOverlayRoot->SetupAttachment(FPVCamera);
    ArtilleryBeamOverlayRoot->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    
    ArtilleryBeamOverlay =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArtilleryBeamOverlay"));
    ArtilleryBeamOverlay->SetupAttachment(ArtilleryBeamOverlayRoot);
    ArtilleryBeamOverlay->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ArtilleryBeamOverlay->SetCastShadow(false);
    ArtilleryBeamOverlay->SetVisibility(false);
    

    LaserMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMarker"));
    LaserMarker->SetupAttachment(RootComponent);
    LaserMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LaserMarker->SetCastShadow(false);
    LaserMarker->SetVisibility(false);
    LaserMarker->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
    
    

}


void ADronePawn::BeginPlay()
{
    Super::BeginPlay();


    CurrentHealth = MaxHealth;
    
    for (TActorIterator<AAntiAirTurret> It(GetWorld()); It; ++It)
    {
        AAntiAirTurret* Turret = *It;
        if (Turret)
        {
            Turret->OnTurretDestroyedByArtillery.AddDynamic(
                this,
                &ADronePawn::OnTurretDestroyedShake
            );
        }
    }

    bControlLocked  = true;
    bEngineStarted  = false;
    bEngineStarting = false;

    FlightMode = EDroneFlightMode::Relax;

    FPVCamera->bCaptureEveryFrame = false;
    FPVCamera->bCaptureOnMovement = false;

    FPVInputRotation = FRotator::ZeroRotator;
    SmoothedFPVRotation = FPVInputRotation;

    FPVCamera->SetRelativeRotation(SmoothedFPVRotation);

    GetWorld()->GetTimerManager().SetTimer(FPVCaptureTimer,this,&ADronePawn::CaptureFPV,0.0714f,true);

    SpringArmBaseRotation = SpringArm->GetRelativeRotation();
    SpringArmBaseLocation = SpringArm->GetRelativeLocation();
    
    BaseFPVShakeStrength   = FPVShakeStrength;
    BaseWindJitterAngle   = WindJitterAngle;
    BaseWindSwayAngle     = WindSwayAngle;
    BaseCameraShakeAngle  = CameraShakeAngle;
    BaseCameraShakeLocation = CameraShakeLocation;
    
    
    BaseAcceleration      = Acceleration;
    BaseYawAcceleration   = YawAcceleration;


    if (WarningAudio && WarningSound)
    {
        WarningAudio->SetSound(WarningSound);
        WarningAudio->SetVolumeMultiplier(0.f);
    }

    if (TurretWarningAudio && TurretWarningSound)
    {
        TurretWarningAudio->SetSound(TurretWarningSound);
        TurretWarningAudio->SetVolumeMultiplier(0.f);
    }

    if (ExplosionAudio && ExplosionSound)
    {
        ExplosionAudio->SetSound(ExplosionSound);
    }

    ATutorialManager* TutorialManager = Cast<ATutorialManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATutorialManager::StaticClass()));

    if (TutorialManager)
    {
        TutorialManager->OnTutorialFinished.AddDynamic(this, &ADronePawn::HandleTutorialFinished);
    }
    else
    {
       
    }
    
    MissionManagerRef = Cast<AMissionManager>(UGameplayStatics::GetActorOfClass(GetWorld(),AMissionManager::StaticClass())
);

    if (!MissionManagerRef)
    {
    ;
    }

}


void ADronePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);



    if (bDestroyed && !bExplosionCameraActive)return;

  
    const FVector Input = bControlLocked ? FVector::ZeroVector : MoveInput;

    FVector CorruptedInput = Input;

    if (bAntiDroneEverTriggered)
    {
        const float Chaos = FMath::Lerp(0.f, 1.f, InterferenceLevel);
        CorruptedInput.X += FMath::Sin(GetWorld()->TimeSeconds * 4.0f) * Chaos * 0.25f;
        CorruptedInput.Y += FMath::Sin(GetWorld()->TimeSeconds * 3.1f) * Chaos * 0.25f;
        CorruptedInput = CorruptedInput.GetClampedToMaxSize(1.f);
    }


    FVector DesiredVelocity =GetActorForwardVector() * CorruptedInput.X +GetActorRightVector()   * CorruptedInput.Y +FVector::UpVector       * CorruptedInput.Z;

  
    float SpeedMul = 1.f;
    float YawMul   = 1.f;

    if (FlightMode == EDroneFlightMode::Relax)
    {
        SpeedMul = RelaxSpeedMultiplier;
        YawMul   = RelaxYawMultiplier;
    }
    else if (FlightMode == EDroneFlightMode::Speed)
    {
        SpeedMul = SpeedModeMultiplier;
    }
    else if (FlightMode == EDroneFlightMode::Land)
    {
        SpeedMul = LandSpeedMultiplier;
        YawMul   = LandYawMultiplier;
        DesiredVelocity.Z *= LandVerticalBoost;
    }

    DesiredVelocity =DesiredVelocity.GetClampedToMaxSize(1.f) * MaxSpeed * SpeedMul;

    const float Interp =DesiredVelocity.IsNearlyZero() ? Deceleration : Acceleration;
    const float DesiredSpeed = DesiredVelocity.Size();
    const float MoveAlpha = FMath::Clamp(DesiredSpeed / (MaxSpeed * SpeedMul),0.f,1.f);
    const float RotateAlpha = FMath::Abs(YawInput);
    const float DesiredAlpha = FMath::Max(MoveAlpha, RotateAlpha);
    const bool bWantsBoost =DesiredAlpha > BoostOnThreshold && !bControlLocked;
    const float BoostInterpSpeed =bWantsBoost ? BoostFadeInSpeed : BoostFadeOutSpeed;

    BoostIntent = FMath::FInterpTo(BoostIntent,bWantsBoost ? 1.f : 0.f,DeltaTime,BoostInterpSpeed);

    if (!bWantsBoost && BoostIntent < BoostOffThreshold)
    {
        BoostIntent = 0.f;
    }



    if (bEngineStarted && BoostAudio && BoostSound)
    {
        float BoostMax =(FlightMode == EDroneFlightMode::Speed)? BoostMaxVolumeSpeed: BoostMaxVolumeRelax;

        const float FinalBoostIntent = BoostIntent;
        const float BoostVol = FMath::Lerp(BoostIdleVolume,BoostMax,FinalBoostIntent);

        BoostAudio->SetVolumeMultiplier(BoostVol);
        BoostAudio->SetPitchMultiplier(FMath::Lerp(1.0f, 1.25f, FinalBoostIntent));
    }


    CurrentVelocity = FMath::VInterpTo(CurrentVelocity, DesiredVelocity, DeltaTime, Interp);

    const float Damping = FMath::Pow(0.98f, DeltaTime * 60.f);
    
    CurrentVelocity *= Damping;
    CurrentVelocity =CurrentVelocity.GetClampedToMaxSize(MaxSpeed * SpeedMul);

    AddActorWorldOffset(CurrentVelocity * DeltaTime, true);


    const float FullSpeedCm = CurrentVelocity.Size();
    const float TargetSpeedKmh = FullSpeedCm * 0.036f;

    SmoothedSpeedKmh = FMath::FInterpTo(SmoothedSpeedKmh,TargetSpeedKmh,DeltaTime,6.f);

    SpeedKmh = FMath::FloorToInt(SmoothedSpeedKmh);

    SpeedNormalized = FMath::Clamp(SmoothedSpeedKmh / MaxSpeedKmhUI,0.f,1.f);


    if (UGameInstance* GI = GetGameInstance())
    {
        if (UDPGameInstance* DP = Cast<UDPGameInstance>(GI))
        {
            DP->SetDroneSpeed01(SpeedNormalized);
        }
    }


    float CorruptedYawInput = YawInput;

    if (bAntiDroneEverTriggered)
    {
        const float Chaos = FMath::Lerp(0.f, 1.f, InterferenceLevel);

        CorruptedYawInput +=FMath::Sin(GetWorld()->TimeSeconds * 5.0f) * Chaos * 0.25f;
        CorruptedYawInput = FMath::Clamp(CorruptedYawInput, -1.f, 1.f);
    }

    if (!bControlLocked)
    {
        const float TargetYaw = CorruptedYawInput * YawSpeed * YawMul;
        const float YawInterp =FMath::IsNearlyZero(TargetYaw) ? YawDeceleration : YawAcceleration;

        CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYaw, DeltaTime, YawInterp);

        AddActorWorldRotation(FRotator(0.f, CurrentYawSpeed * DeltaTime, 0.f));
    }

  
    if (FPVCamera && bEngineStarted)
    {
        FPVShakeTime += DeltaTime * FPVShakeSpeed;

        const float SpeedAlpha =FMath::Clamp(CurrentVelocity.Size() / MaxSpeed, 0.f, 1.f);

        const float FPVInterferenceMul = FMath::Lerp(1.f, 4.0f, InterferenceLevel);
        const float Shake = FPVShakeStrength * SpeedAlpha * FPVInterferenceMul;

        const FRotator ShakeRot(FMath::Sin(FPVShakeTime * 1.7f) * Shake,FMath::Sin(FPVShakeTime * 2.3f) * Shake,0.f);
        FRotator TargetRotation = FPVInputRotation + ShakeRot;

        SmoothedFPVRotation = FMath::RInterpTo(SmoothedFPVRotation,TargetRotation,DeltaTime,8.0f   );

        FPVCamera->SetRelativeRotation(SmoothedFPVRotation);
    }

 
    UpdateAltitude();
    UpdateBattery(DeltaTime);

    UpdateDroneTilt(DeltaTime);
    UpdateWindSway(DeltaTime);
    UpdateMainCameraTilt(DeltaTime);
    UpdateMainCameraShake(DeltaTime);

    
    if (bExplosionCameraActive && SpringArm)
    {
        ExplosionCameraTime += DeltaTime;

        float Alpha = ExplosionCameraTime / ExplosionCameraDuration;
        Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

     
        float Shock = FMath::Sin(Alpha * PI);
        float Fade = 1.f - Alpha;
        float RotStrength = ExplosionCameraAngle * Shock * Fade;
        float Pitch = FMath::Sin(ExplosionCameraTime * 60.f) * RotStrength;
        float Roll  = FMath::Cos(ExplosionCameraTime * 45.f) * RotStrength;

        CameraShakeRotation = FRotator(Pitch, 0.f, Roll);


        float BackKick = ExplosionCameraLocation * Shock * Fade;

        CameraShakeOffset =-GetActorForwardVector() * BackKick;


        if (Alpha >= 1.f)
        {
            bExplosionCameraActive = false;
            CameraShakeRotation = FRotator::ZeroRotator;
            CameraShakeOffset = FVector::ZeroVector;
        }
    }
    


    CameraLookOffset.Yaw   += CameraLookYawInput   * MouseLookYawSpeed   * DeltaTime;
    CameraLookOffset.Pitch += CameraLookPitchInput * MouseLookPitchSpeed * DeltaTime;

    CameraLookOffset.Yaw   = FMath::Clamp(CameraLookOffset.Yaw,   -MaxMouseLookYaw,   MaxMouseLookYaw);
    CameraLookOffset.Pitch = FMath::Clamp(CameraLookOffset.Pitch, -MaxMouseLookPitch, MaxMouseLookPitch);

    CameraLookOffset = FMath::RInterpTo(CameraLookOffset,FRotator::ZeroRotator,DeltaTime,MouseLookReturnSpeed);


    if (SpringArm)
    {

        const bool bAiming =bReconScanning || bArtilleryDesignating;

        FVector TargetOffset = FVector::ZeroVector;

        if (bAiming)
        {
            TargetOffset = FVector(0.f,-AimCameraSideOffset, -30.f  );
        }
        
        CurrentAimOffset = FMath::VInterpTo(CurrentAimOffset,TargetOffset,DeltaTime,AimCameraInterpSpeed);

        SpringArm->SetRelativeRotation(SpringArmBaseRotation+ CameraTiltRotation+ CameraShakeRotation+ CameraLookOffset);
       
        float ForwardSpeed =FVector::DotProduct(CurrentVelocity,GetActorForwardVector());
        float RightSpeed =FVector::DotProduct(CurrentVelocity,GetActorRightVector());


        float ForwardLag = -ForwardSpeed * 0.015f;
        float SideLag    = -RightSpeed  * 0.008f;

        FVector VelocityLag(ForwardLag,SideLag,0.);

        SpringArm->SetRelativeLocation(SpringArmBaseLocation+ CameraShakeOffset+ CurrentAimOffset+ VelocityLag);
    }


    float TargetInterference;
    float InterpSpeed;

    if (bFixedOnce)
    {
        TargetInterference = 0.f;
        InterpSpeed = InterferenceFallSpeed;
    }
    else if (bInsideFixZone)
    {
        TargetInterference = 0.f;
        InterpSpeed = FixSpeed;
    }
    else if (bInsideAntiDroneZone)
    {
        TargetInterference = 1.f;
        InterpSpeed = InterferenceRiseSpeed;
    }
    else if (bAntiDroneEverTriggered)
    {
        TargetInterference = 1.f;
        InterpSpeed = InterferenceFallSpeed;
    }
    else
    {
        TargetInterference = 0.f;
        InterpSpeed = InterferenceFallSpeed;
    }

    InterferenceLevel = FMath::FInterpTo(InterferenceLevel,TargetInterference,DeltaTime,InterpSpeed);
    


    const float ZoneInterference = InterferenceLevel;
    const float HealthAlpha = 1.f - (CurrentHealth / MaxHealth);
    const float HealthCurve = FMath::Pow(HealthAlpha, 1.6f);
    const float HealthInterference = FMath::Clamp(HealthCurve * 1.2f,0.f,1);

    

    InterferenceLevel = FMath::Clamp(FMath::Max(ZoneInterference, HealthInterference),0.f,1.f);

    if (bInsideFixZone && CurrentHealth < MaxHealth)
    {
        CurrentHealth = FMath::Clamp(CurrentHealth + HealthHealSpeed * DeltaTime,0.f,MaxHealth);
    }
    if (!bFixedOnce && bInsideFixZone && InterferenceLevel < 0.02f)
    {
        InterferenceLevel = 0.f;

        bAntiDroneEverTriggered = false;
        bFixedOnce = true; 

        WindSwayRotation     = FRotator::ZeroRotator;
        CameraShakeRotation  = FRotator::ZeroRotator;
        CameraShakeOffset    = FVector::ZeroVector;
        FPVShakeTime         = 0.f;
        WindSwayTime         = 0.f;
    }



    const float SafeInterference = bInsideFixZone ? 0.f : InterferenceLevel;
    FPVShakeStrength      = BaseFPVShakeStrength      * FMath::Lerp(1.f, 4.5f, SafeInterference);
    WindJitterAngle       = BaseWindJitterAngle       * FMath::Lerp(1.f, 5.0f, SafeInterference);
    WindSwayAngle         = BaseWindSwayAngle         * FMath::Lerp(1.f, 3.5f, SafeInterference);
    CameraShakeAngle      = BaseCameraShakeAngle      * FMath::Lerp(1.f, 3.0f, SafeInterference);
    CameraShakeLocation   = BaseCameraShakeLocation   * FMath::Lerp(1.f, 2.5f, SafeInterference);


    
    const bool bControlCorrupted = (InterferenceLevel > 0.f) && !bFixedOnce;

    if (WarningAudio && WarningSound)
    {
        if (bControlCorrupted)
        {
            
            if (!WarningAudio->IsPlaying())
                WarningAudio->Play();

            const float TargetVolume = WarningMaxVolume;
            const float NewVol = FMath::FInterpTo(WarningAudio->VolumeMultiplier,TargetVolume,DeltaTime,WarningFadeSpeed);

            WarningAudio->SetVolumeMultiplier(NewVol);
        }
        else
        {
            if (WarningAudio->IsPlaying())
            {
                const float NewVol = FMath::FInterpTo(WarningAudio->VolumeMultiplier,0.f,DeltaTime,WarningFadeSpeed);
                
                WarningAudio->SetVolumeMultiplier(NewVol);

                if (NewVol < 0.01f)WarningAudio->Stop();
            }
        }
    }




    if (TurretWarningAudio && TurretWarningSound)
    {
        if (bUnderTurretFire)
        {
            if (!TurretWarningAudio->IsPlaying())
                TurretWarningAudio->Play();

            const float NewVol = FMath::FInterpTo(TurretWarningAudio->VolumeMultiplier,1.0f,       DeltaTime,5.f);

            TurretWarningAudio->SetVolumeMultiplier(NewVol);
            
        }
        else
        {
            if (TurretWarningAudio->IsPlaying())
            {
                const float NewVol = FMath::FInterpTo(TurretWarningAudio->VolumeMultiplier,0.f,DeltaTime,5.f);

                TurretWarningAudio->SetVolumeMultiplier(NewVol);

                if (NewVol < 0.01f)TurretWarningAudio->Stop();
                
            }
        }
    }

    if (bReconScanning && bArtilleryDesignating)
    {
     
        SetStopArtilleryDesignate();
    }

    if (bReconScanning)
    {
        PerformReconTrace(DeltaTime);
    }


    if (bArtilleryDesignating)
    {
        PerformArtilleryTrace(DeltaTime);
    }
   
}

void ADronePawn::SetMoveForward(float V) { MoveInput.X = V; }
void ADronePawn::SetMoveRight (float V) { MoveInput.Y = V; }
void ADronePawn::SetMoveUp    (float V) { MoveInput.Z = V; }
void ADronePawn::SetYawInput  (float V) { YawInput   = V; }
void ADronePawn::SetFPVLookUp(float V)

{
    FPVInputRotation.Pitch = FMath::Clamp(FPVInputRotation.Pitch + V * FPVPitchSpeed * GetWorld()->GetDeltaSeconds(),MinFPVPitch,MaxFPVPitch);
}

void ADronePawn::SetFPVTurn(float V)
{
    
    FPVInputRotation.Yaw = FMath::Clamp(FPVInputRotation.Yaw + V * FPVYawSpeed * GetWorld()->GetDeltaSeconds(),MinFPVYaw,MaxFPVYaw);
}

void ADronePawn::SetCameraLookYaw(float V)   { CameraLookYawInput   = V; }
void ADronePawn::SetCameraLookPitch(float V) { CameraLookPitchInput = V; }



void ADronePawn::UpdateDroneTilt(float DeltaTime)
{
    const FRotator R = Mesh->GetRelativeRotation();

    if (CurrentVelocity.SizeSquared() < 10.f)
    {
        Mesh->SetRelativeRotation(FMath::RInterpTo(R, FRotator(0.f, R.Yaw, 0.f), DeltaTime, TiltInterpSpeed));return;
    }

    const FVector L =GetActorTransform().InverseTransformVector(CurrentVelocity);

    const float Ref = MaxSpeed * 0.6f;
    const FVector D = L.GetClampedToMaxSize(Ref) / Ref;

    float TiltMul = 1.f;
    if (FlightMode == EDroneFlightMode::Relax) TiltMul = RelaxTiltMultiplier;
    else if (FlightMode == EDroneFlightMode::Land) TiltMul = LandTiltMultiplier;

    Mesh->SetRelativeRotation(FMath::RInterpTo(R,FRotator(D.Y * MaxTiltAngle * TiltMul,R.Yaw,D.X * MaxTiltAngle * TiltMul),DeltaTime,TiltInterpSpeed));
}



void ADronePawn::OnDroneHit(UPrimitiveComponent*,AActor*,UPrimitiveComponent*,FVector,const FHitResult& Hit)

{
    if (FlightMode == EDroneFlightMode::Land)
    {
        CurrentVelocity = FVector::ZeroVector;
        return;
    }

    float BounceStrength = 600.f;
    float Damping = 0.75f;
    float CollisionVolume = 0.6f;

    if (FlightMode == EDroneFlightMode::Relax)
    {
        BounceStrength = 350.f;
        Damping = 0.82f;
        CollisionVolume = 0.45f;
    }
    else if (FlightMode == EDroneFlightMode::Speed)
    {
        BounceStrength = 750.f;
        Damping = 0.65f;
        CollisionVolume = 0.75f;  
    }


    if (CollisionAudio && CollisionSound)
    {
        CollisionAudio->SetSound(CollisionSound);
        CollisionAudio->SetVolumeMultiplier(CollisionVolume);
        CollisionAudio->Play();
    }
    
    if (HitWarningAudio && HitWarningSound)
    {
        HitWarningAudio->SetSound(HitWarningSound);
        HitWarningAudio->SetVolumeMultiplier(1.0f);
        HitWarningAudio->Play();

        GetWorld()->GetTimerManager().ClearTimer(HitWarningTimer);
        GetWorld()->GetTimerManager().SetTimer(HitWarningTimer,this,&ADronePawn::StopHitWarningSound,1.35f,false);
    }

    CurrentVelocity += Hit.ImpactNormal * BounceStrength;
    CurrentVelocity *= Damping;

    bControlLocked = true;

    GetWorld()->GetTimerManager().SetTimer(ControlLockTimer,this,&ADronePawn::UnlockControl,ControlLockTime,false);

    OnDroneCrashed.Broadcast();
    
}
void ADronePawn::StopHitWarningSound()
{
    if (HitWarningAudio)
    {
        HitWarningAudio->Stop();
    }
}


void ADronePawn::CaptureFPV()
{
    if (FPVCamera && bEngineStarted)
        FPVCamera->CaptureScene();
}

void ADronePawn::UpdateAltitude()
{
    FVector Start = GetActorLocation();
    FVector End   = Start - FVector(0.f, 0.f, TraceLength);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        const float DistanceCm = (Start - Hit.Location).Size();
        AltitudeMeters = FMath::RoundToFloat((DistanceCm / 100.f) * 10.f) / 10.f;
        AltitudeNormalized = FMath::Clamp(AltitudeMeters / MaxAltitudeMeters, 0.f, 1.f);
    }
}

void ADronePawn::SetToggleFPV()
{
    bFPVFullscreen = !bFPVFullscreen;
}

void ADronePawn::SetCycleFlightMode()
{
    switch (FlightMode)
    {
        case EDroneFlightMode::Relax: FlightMode = EDroneFlightMode::Speed; break;
        case EDroneFlightMode::Speed: FlightMode = EDroneFlightMode::Land;  break;
        default:FlightMode = EDroneFlightMode::Relax; break;
    }
}

void ADronePawn::UpdateMainCameraTilt(float DeltaTime)
{
    if (!SpringArm || !Mesh) return;

    const FRotator MeshRot = Mesh->GetRelativeRotation();
    const FRotator TargetTilt(-MeshRot.Roll  * CameraTiltStrength,0.f,MeshRot.Pitch * CameraTiltStrength);

    CameraTiltRotation = FMath::RInterpTo(CameraTiltRotation,TargetTilt,DeltaTime,CameraTiltInterpSpeed);

  
}

void ADronePawn::SetStartEngine()
{
    if (bEngineStarted || bEngineStarting) return;

    bEngineStarting = true;
    bControlLocked = true;

    if (TakeoffAudio && TakeoffSound)
    {
        TakeoffAudio->SetSound(TakeoffSound);
        TakeoffAudio->SetVolumeMultiplier(0.65f);
        TakeoffAudio->Play();
        
        const float TakeoffDuration = TakeoffSound->GetDuration();
        
        GetWorld()->GetTimerManager().SetTimer(TakeoffToHoverTimer,this,&ADronePawn::StartHoverSound,TakeoffDuration,false);
        
    }

    if (BoostAudio && BoostSound)
    {
        BoostAudio->SetSound(BoostSound);
        BoostAudio->Play();
        BoostAudio->SetVolumeMultiplier(BoostIdleVolume * 0.5f); 
    }
}
void ADronePawn::OnEngineTakeoffFinished()
{
    bEngineStarting = false;
    bEngineStarted = true;
    bControlLocked = false;
}
void ADronePawn::UnlockControl()
{
    bControlLocked = false;
}

void ADronePawn::UpdateBattery(float DeltaTime)
{
    if (!bEngineStarted) return;         
    if (BatteryPercent <= 0.f) return;

    const float DrainMul =(FlightMode == EDroneFlightMode::Speed) ? SpeedModeDrainMultiplier :(FlightMode == EDroneFlightMode::Land)  ? LandingDrainMultiplier : 1.f;
    const float DrainPerSecond = 100.f / BatteryLifeSeconds;

    BatteryPercent -= DrainPerSecond * DrainMul * DeltaTime;
    BatteryPercent = FMath::Clamp(BatteryPercent, 0.f, 100.f);

    if (BatteryPercent <= LowBatteryPercent && FlightMode != EDroneFlightMode::Land)
    {
        FlightMode = EDroneFlightMode::Land;
    }
}



void ADronePawn::UpdateWindSway(float DeltaTime)
{
    if (!Mesh) return;
    if (!bEngineStarted) return;
    if (FlightMode == EDroneFlightMode::Land) return;

    const float Speed = CurrentVelocity.Size();

    // ---- Speed → 0..1 ----
    const float InterferenceMul = FMath::Lerp(1.f, 1.8f, InterferenceLevel);

    const float SpeedAlpha = FMath::GetMappedRangeValueClamped(FVector2D(WindSwaySpeedStart, WindSwaySpeedMax),FVector2D(0.f, 1.f),Speed) * InterferenceMul;
    if (SpeedAlpha <= 0.f)return;
    
    WindSwayTime += DeltaTime * WindSwayFrequency;
    
    const float SwayMul = FMath::Lerp(1.f, 1.6f, InterferenceLevel);
    const float SwayRoll =FMath::PerlinNoise1D(WindSwayTime) *WindSwayAngle * SpeedAlpha * SwayMul;
    const float SwayPitch =FMath::PerlinNoise1D(WindSwayTime + 31.7f) *WindSwayAngle * 0.45f * SpeedAlpha * SwayMul;
    const float JitterTime = WindSwayTime * WindJitterFrequency;
    const float JitterMul = FMath::Lerp(1.f, 5.0f, InterferenceLevel);
    const float JitterRoll =FMath::Sin(JitterTime * 2.3f) *WindJitterAngle * SpeedAlpha * JitterMul;
    const float JitterPitch =FMath::Sin(JitterTime * 3.7f) *WindJitterAngle * 0.8f * SpeedAlpha * JitterMul;

    const FRotator Target(SwayPitch + JitterPitch,0.f,SwayRoll  + JitterRoll);

  
    WindSwayRotation = FMath::RInterpTo(WindSwayRotation,Target,DeltaTime,6.0f);


    Mesh->AddRelativeRotation(WindSwayRotation);
}


void ADronePawn::UpdateMainCameraShake(float DeltaTime)
{
    if (!SpringArm) return;
    if (!bEngineStarted) return;

    const float Speed = CurrentVelocity.Size();

    const float SpeedAlpha = FMath::GetMappedRangeValueClamped(FVector2D(CameraShakeSpeedStart, CameraShakeSpeedMax),FVector2D(0.f, 1.f),Speed);

    if (SpeedAlpha <= 0.f)return;

    CameraShakeTime += DeltaTime * CameraShakeSpeed;

    const float CamInterferenceMul = FMath::Lerp(1.f, 3.0f, InterferenceLevel);
    const float Roll =FMath::Sin(CameraShakeTime * 1.3f) * CameraShakeAngle * SpeedAlpha * CamInterferenceMul;
    const float Pitch =FMath::Sin(CameraShakeTime * 2.1f) *CameraShakeAngle * 0.6f * SpeedAlpha * CamInterferenceMul;
    const FRotator TargetShake(Pitch, 0.f, Roll);

    CameraShakeRotation = FMath::RInterpTo(CameraShakeRotation,TargetShake,DeltaTime,6.f);
    




    const float LocNoiseX = FMath::PerlinNoise1D(CameraShakeTime + 11.1f);
    const float LocNoiseY = FMath::PerlinNoise1D(CameraShakeTime + 22.2f);
    const float LocNoiseZ = FMath::PerlinNoise1D(CameraShakeTime + 33.3f);

    const FVector TargetOffset =FVector(LocNoiseX, LocNoiseY, LocNoiseZ) *CameraShakeLocation * SpeedAlpha;

    CameraShakeOffset = FMath::VInterpTo(CameraShakeOffset,TargetOffset,DeltaTime,10.f);


}

void ADronePawn::StartHoverSound()
{
    if (HoverAudio && HoverSound)
    {
        HoverAudio->SetSound(HoverSound);
        HoverAudio->SetVolumeMultiplier(HoverVolume);
        HoverAudio->Play();
    }

    if (BoostAudio && BoostSound)
    {
        BoostAudio->SetSound(BoostSound);
        BoostAudio->Play();
        BoostAudio->SetVolumeMultiplier(BoostIdleVolume);
    }
}

void ADronePawn::StartFixing()
{
    bInsideFixZone = true;
    bFixedOnce = false; 
}

void ADronePawn::StopFixing()
{
    bInsideFixZone = false;

}

void ADronePawn::SetUnderTurretFire(bool bState)
{
    bUnderTurretFire = bState;
}


void ADronePawn::ApplyDamage(float DamageAmount)
{
    if (bDestroyed) return;

    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

    if (CurrentHealth <= 0.f)
    {
    
        Explode();
    }
} 

void ADronePawn::Explode()
{
    if (bDestroyed) return;
    bDestroyed = true;

    bReconScanning = false;
    bArtilleryDesignating = false;

    if (CurrentArtilleryTarget)
    {
        CurrentArtilleryTarget->ProcessDesignate(0.f, false);
        CurrentArtilleryTarget = nullptr;
    }
 


    const FVector Location = GetActorLocation();

    bControlLocked  = true;
    bEngineStarted  = false;
    bEngineStarting = false;
    InterferenceLevel = 0.f;
    bInsideAntiDroneZone = false;
    bAntiDroneEverTriggered = false;
    bInsideFixZone = false;
    bFixedOnce = false;

    if (ExplosionNiagara)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ExplosionNiagara,Location,GetActorRotation(),ExplosionScale);
    }
    
    if (ExplosionAudio && ExplosionSound)
    {
        ExplosionAudio->SetWorldLocation(Location);
        ExplosionAudio->SetVolumeMultiplier(3.0f); 
        ExplosionAudio->Play(0.f);
    }

    CurrentVelocity = FVector::ZeroVector;

    Mesh->SetVisibility(false);
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ExplosionCameraTime = 0.f;
    bExplosionCameraActive = true;


    CameraShakeRotation = FRotator(-ExplosionCameraAngle * 2.f, 0.f, ExplosionCameraAngle);
    CameraShakeOffset   = -GetActorForwardVector() * (ExplosionCameraLocation * 1.5f);
    

    if (TakeoffAudio)        TakeoffAudio->Stop();
    if (HoverAudio)          HoverAudio->Stop();
    if (BoostAudio)          BoostAudio->Stop();
    if (WarningAudio)        WarningAudio->Stop();
    if (TurretWarningAudio)  TurretWarningAudio->Stop();

    if (LaserAudio)
    {
        LaserAudio->Stop();
    }

    GetWorld()->GetTimerManager().ClearTimer(RespawnAfterDeathTimer);

    if (MissionWidgetComponent)
    {
        MissionWidgetComponent->SetVisibility(false);
    }

    OnDroneDestroyed.Broadcast();
    
}


void ADronePawn::RespawnDrone(const FVector& NewLocation, const FRotator& NewRotation)
{

    

    SetActorLocationAndRotation(NewLocation,NewRotation,false,nullptr,ETeleportType::TeleportPhysics);


    bDestroyed = false;
    bControlLocked = false;
    bEngineStarted = true;
    bEngineStarting = false;

    SetActorTickEnabled(true);

    CurrentVelocity = FVector::ZeroVector;
    CurrentYawSpeed = 0.f;
    CurrentHealth = MaxHealth;
    BatteryPercent = 100.f;
    
    FlightMode = EDroneFlightMode::Relax;

  
    InterferenceLevel = 0.f;
    bInsideAntiDroneZone = false;
    bAntiDroneEverTriggered = false;
    bInsideFixZone = false;
    bFixedOnce = false;


    CameraShakeRotation = FRotator::ZeroRotator;
    CameraShakeOffset = FVector::ZeroVector;
    WindSwayRotation = FRotator::ZeroRotator;
    FPVShakeTime = 0.f;
    WindSwayTime = 0.f;
    
    if (Mesh)Mesh->SetVisibility(true);

    if (BoxCollision)BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

  
    if (TakeoffAudio)       TakeoffAudio->Stop();
    if (HoverAudio)         HoverAudio->Stop();
    if (BoostAudio)         BoostAudio->Stop();
    if (WarningAudio)       WarningAudio->Stop();
    if (TurretWarningAudio) TurretWarningAudio->Stop();
    if (LaserAudio)
    {
        LaserAudio->Stop();
    }

    bLaserLocked = false;

    if (MissionWidgetComponent)
    {
        MissionWidgetComponent->SetVisibility(true);
    }


    StartHoverSound();
    
}



void ADronePawn::HandleTutorialFinished()
{


    GetWorld()->GetTimerManager().SetTimer(MissionDelayTimer,this,&ADronePawn::ShowMissionWidget,3.0f,   false);
    
}

void ADronePawn::ShowMissionWidget()
{
    if (MissionWidgetComponent)
    {
        MissionWidgetComponent->SetVisibility(true);
    }

    if (MissionManagerRef)
    {
        MissionManagerRef->StartCheckpointMission();
    }
}

void ADronePawn::StartReconScan()
{
    if (!bEngineStarted) return;
    if (bDestroyed && !bExplosionCameraActive) return;
    if (!bFPVFullscreen) return;
    if (bArtilleryDesignating)
    {
        return;
    }

    bReconScanning = true;
}

void ADronePawn::StopReconScan()
{
    bReconScanning = false;

    if (ReconBeamOverlay)
    {
        ReconBeamOverlay->SetVisibility(false);
    }


    if (LaserAudio)
    {
        LaserAudio->Stop();
    }

    bLaserLocked = false;

    if (ReconBeam)
    {
        ReconBeam->SetVisibility(false);
        
        
    }

    if (LaserMarker)
    {
        LaserMarker->SetVisibility(false);
    }
}

void ADronePawn::PerformReconTrace(float DeltaTime)
{
    if (!FPVCamera) return;
    if (!bReconScanning) return;

    FVector Start = FPVCamera->GetComponentLocation();
    FVector Forward = FPVCamera->GetForwardVector();
    FVector End = Start + Forward * ReconTraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params);


    AReconTarget* Target = nullptr;

    if (bHit)
    {
        Target = Cast<AReconTarget>(Hit.GetActor());
        if (!Target && Hit.GetComponent())Target = Cast<AReconTarget>(Hit.GetComponent()->GetOwner());
    }


    bool bIsRecon = (Target != nullptr);

    UpdateLaserSound(bIsRecon);

    float Progress = 0.f;
    if (MissionManagerRef && MissionManagerRef->ReconMission)
    {
        Progress = MissionManagerRef->ReconMission->ScanProgress;
    }

    UpdateLaserMarker(Hit, bIsRecon, false, Progress);


    if (ReconBeam)
    {
        ReconBeam->SetVisibility(true);

        float BeamLength = ReconTraceDistance;

        FVector Offset = FVector(BeamLength * 0.5f, 0.f, 0.f);
        ReconBeam->SetRelativeLocation(Offset);

        FVector Scale = FVector(BeamLength / 100.f, 1.f, 1.f);
        ReconBeam->SetRelativeScale3D(Scale);

  
    }
    
    if (ReconBeamOverlay)
    {
        ReconBeamOverlay->SetVisibility(true);

        float OverlayLength = ReconTraceDistance; 

        FVector OverlayOffset =FVector(OverlayLength * 0.5f, 0.f, 0.f);
        FVector OverlayScale =FVector(OverlayLength / 100.f, 1.f, 1.f)* ReconOverlayScale;

        ReconBeamOverlay->SetRelativeLocation(OverlayOffset);
        ReconBeamOverlay->SetRelativeScale3D(OverlayScale);
    }


    Target = Cast<AReconTarget>(Hit.GetActor());

    if (MissionManagerRef && MissionManagerRef->ReconMission)
    {
        if (Target)
        {
            MissionManagerRef->ReconMission->UpdateScanProgress(DeltaTime, true);
            Target->ProcessScan(DeltaTime, true);
        }
        else
        {
            MissionManagerRef->ReconMission->UpdateScanProgress(DeltaTime, false);

            for (TActorIterator<AReconTarget> It(GetWorld()); It; ++It)
            {
                It->ProcessScan(DeltaTime, false);
            }
        }
    }
}

void ADronePawn::SetStartArtilleryDesignate()
{
    
    if (!bEngineStarted)

    {
        return;
    }
    if (bDestroyed && !bExplosionCameraActive) return;
    
    if (!bFPVFullscreen)
    {
        return;
    }

    if (bReconScanning)
    {
        return;
    }
    bArtilleryDesignating = true;

}
void ADronePawn::SetStopArtilleryDesignate()
{
    bArtilleryDesignating = false;

    if (LaserAudio)
    {
        LaserAudio->Stop();
    }

    bLaserLocked = false;

    if (ArtilleryBeam)
    {
        ArtilleryBeam->SetVisibility(false);
    }

    if (ArtilleryBeamOverlay)
    {
        ArtilleryBeamOverlay->SetVisibility(false);
    }

    if (MissionManagerRef && MissionManagerRef->ArtilleryMission)
    {
        MissionManagerRef->ArtilleryMission->UpdateHoldProgress(0.f, false);
    }

    if (CurrentArtilleryTarget)
    {
        CurrentArtilleryTarget->ProcessDesignate(0.f, false);
        CurrentArtilleryTarget = nullptr;
    }

    if (LaserMarker)
    {
        LaserMarker->SetVisibility(false);
    }
}
void ADronePawn::PerformArtilleryTrace(float DeltaTime)
{
    if (!FPVCamera) return;
    if (!bArtilleryDesignating) return;

    FVector Start = FPVCamera->GetComponentLocation();
    FVector Forward = FPVCamera->GetForwardVector();
    FVector End = Start + Forward * ArtilleryTraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params);


    AArtilleryTarget* NewTarget = nullptr;

    if (bHit)
    {
        if (Hit.GetActor())NewTarget = Cast<AArtilleryTarget>(Hit.GetActor());
        if (!NewTarget && Hit.GetComponent())NewTarget = Cast<AArtilleryTarget>(Hit.GetComponent()->GetOwner());
        
    }

    const bool bHitTarget = (NewTarget != nullptr);

    UpdateLaserSound(bHitTarget);

    float Progress = 0.f;
    if (MissionManagerRef && MissionManagerRef->ArtilleryMission)
    {
        Progress = MissionManagerRef->ArtilleryMission->HoldProgress;
    }

    UpdateLaserMarker(Hit, false, bHitTarget, Progress);


    if (NewTarget != CurrentArtilleryTarget)
    {
        if (CurrentArtilleryTarget)CurrentArtilleryTarget->ProcessDesignate(0.f, false);

        CurrentArtilleryTarget = NewTarget;
    }


    if (MissionManagerRef && MissionManagerRef->ArtilleryMission)
    {
        MissionManagerRef->ArtilleryMission->UpdateHoldProgress(DeltaTime,bHitTarget);
    }

    if (CurrentArtilleryTarget)
    {
        CurrentArtilleryTarget->ProcessDesignate(DeltaTime, true);
    }


    if (ArtilleryBeam)
    {
        ArtilleryBeam->SetVisibility(true);

        float BeamLength = ArtilleryTraceDistance;

        FVector Offset = FVector(BeamLength * 0.5f, 0.f, 0.f);
        ArtilleryBeam->SetRelativeLocation(Offset);

        FVector Scale = FVector(BeamLength /100.f, 1.f, 1.f);
        ArtilleryBeam->SetRelativeScale3D(Scale);
        
    }


    if (ArtilleryBeamOverlay)
    {
        ArtilleryBeamOverlay->SetVisibility(true);

        float OverlayLength = ArtilleryTraceDistance;
        
        FVector OverlayOffset =FVector(OverlayLength * 0.5f, 0.f, 0.f);
        FVector OverlayScale =FVector(OverlayLength / 100.f, 1.f, 1.f)* ArtilleryOverlayScale;

        ArtilleryBeamOverlay->SetRelativeLocation(OverlayOffset);
        ArtilleryBeamOverlay->SetRelativeScale3D(OverlayScale);
    }
    
}
void ADronePawn::OnTurretDestroyedShake()
{
    ExplosionCameraTime = 0.f;
    bExplosionCameraActive = true;

    CameraShakeRotation = FRotator(-ExplosionCameraAngle * 1.5f,0.f,ExplosionCameraAngle);
    CameraShakeOffset =-GetActorForwardVector() * (ExplosionCameraLocation * 1.2f);
    
}

void ADronePawn::UpdateLaserMarker(const FHitResult& Hit,bool bIsRecon,bool bIsArtillery,float HoldProgress)

{
    if (!LaserMarker) return;

    if (!Hit.bBlockingHit)
    {
        LaserMarker->SetVisibility(false);
        return;
    }

    LaserMarker->SetVisibility(true);


    const float SurfaceOffset = 2.0f;
    const FVector OffsetLocation =Hit.ImpactPoint + Hit.ImpactNormal * SurfaceOffset;

    LaserMarker->SetWorldLocation(OffsetLocation);

    const bool bHitTarget = (bIsRecon || bIsArtillery);


    const FQuat BaseQuat =FRotationMatrix::MakeFromZ(Hit.ImpactNormal).ToQuat();

    float BaseSpinSpeed = 60.f;
    float SpinMultiplier = bHitTarget ? 2.5f : 1.0f;
    float FinalSpinSpeed = BaseSpinSpeed * SpinMultiplier;

    MarkerSpinAngle += FinalSpinSpeed * GetWorld()->GetDeltaSeconds();

    const FQuat SpinQuat(Hit.ImpactNormal,FMath::DegreesToRadians(MarkerSpinAngle));

    LaserMarker->SetWorldRotation(SpinQuat * BaseQuat);


    float PulseSpeed     = bHitTarget ? 6.f : 3.f;
    float PulseAmplitude = bHitTarget ? 0.18f : 0.08f;

    MarkerPulseTime += GetWorld()->GetDeltaSeconds() * PulseSpeed;
    float Pulse = 1.f + FMath::Sin(MarkerPulseTime) * PulseAmplitude;
    float DistCm = Hit.Distance;
    if (DistCm <= 0.f)
    {
        DistCm = (Hit.ImpactPoint - Hit.TraceStart).Size();
    }

    const float MaxDist = FMath::Max(1.f, MarkerMaxTraceDistance);
    float DistAlpha = FMath::Clamp(DistCm / MaxDist, 0.f, 1.f);
    float BaseSize = FMath::Lerp(MarkerScaleNear, MarkerScaleFar, DistAlpha);


    LaserMarker->SetWorldScale3D(FVector(BaseSize * Pulse));
    
    if (bArtilleryDesignating && MI_MarkerRed)
    {
        LaserMarker->SetMaterial(0, MI_MarkerRed);
    }
    else if (bReconScanning && MI_MarkerBlue)
    {
        LaserMarker->SetMaterial(0, MI_MarkerBlue);
    }
    
}
void ADronePawn::UpdateLaserSound(bool bHitTarget)
{
    if (!LaserAudio) return;

    USoundBase* DesiredSound =bHitTarget ? LaserLockSound : LaserIdleSound;

    if (!DesiredSound) return;
    
    if (LaserAudio->Sound == DesiredSound &&LaserAudio->IsPlaying())
    {
        return;
    }

 
    LaserAudio->Stop();
    LaserAudio->SetSound(DesiredSound);
    LaserAudio->SetVolumeMultiplier(bHitTarget ? 2.0f : 0.9f
    );

    LaserAudio->SetPitchMultiplier(bHitTarget ? 1.2f : 1.0f
    );

    LaserAudio->Play();
    bLaserLocked = bHitTarget;
}