#include "AntiAirTurret.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DroneProject/Pawn/DronePawn.h"
#include "TurretAnimInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AAntiAirTurret::AAntiAirTurret()
{
    PrimaryActorTick.bCanEverTick = true;
    
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);


    TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TurretMesh"));
    TurretMesh->SetupAttachment(Root);
    TurretMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    TurretMesh->SetCollisionObjectType(ECC_WorldStatic);
    TurretMesh->SetCollisionResponseToAllChannels(ECR_Block);
    TurretMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    TurretMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

  
    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    DetectionSphere->SetupAttachment(Root);
    DetectionSphere->InitSphereRadius(4000.f);
    DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
    DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    DetectionSphere->SetGenerateOverlapEvents(true);
    

    MuzzleFlashComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFlash"));
    MuzzleFlashComponent->SetupAttachment(TurretMesh, TEXT("Muzzle"));
    MuzzleFlashComponent->SetAutoActivate(false);


    FireAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("FireAudio"));
    FireAudio->SetupAttachment(TurretMesh, TEXT("Muzzle"));
    FireAudio->bAutoActivate = false;
    FireAudio->bIsUISound = false;
    
    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this,&AAntiAirTurret::OnEnterSphere);
    DetectionSphere->OnComponentEndOverlap.AddDynamic(this,&AAntiAirTurret::OnExitSphere);

    TargetDrone = nullptr;
}

void AAntiAirTurret::BeginPlay() 
{
    Super::BeginPlay();
    DetectionSphere->SetSphereRadius(DetectionRadius);


    if (MuzzleFlashComponent)
    {
        MuzzleFlashComponent->SetRelativeScale3D(
            FVector(MuzzleFlashScale)
        );
    }
    
 
    if (TurretMesh)
    {
        OriginalMaterials.Empty();
    
        const int32 MatCount = TurretMesh->GetNumMaterials();

        for (int32 i = 0; i < MatCount; ++i)
        {
            OriginalMaterials.Add(TurretMesh->GetMaterial(i));
        }
    }


}

void AAntiAirTurret::Tick(float DeltaTime)
{
    
    
    Super::Tick(DeltaTime);

    if (bDestroyed)
        return;

    if (!TargetDrone || !IsValid(TargetDrone))
        return;

    const FVector TurretLocation = GetActorLocation();
    const FVector TargetLocation = TargetDrone->Mesh->GetSocketLocation("Drone_Target");
    const FVector ToTarget = TargetLocation - TurretLocation;
    const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(TurretLocation, TargetLocation);
    const float TargetYaw = (LookAt - GetActorRotation()).GetNormalized().Yaw;
    const float DistanceXY = FVector(ToTarget.X, ToTarget.Y, 0.f).Size();
    const float SafeDistance = FMath::Max(DistanceXY, MinDistanceXY);
    float TargetPitch = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Z, SafeDistance));
    TargetPitch = FMath::Clamp(TargetPitch, MinPitch, MaxPitch);
    CurrentYaw = FMath::FInterpTo(CurrentYaw,TargetYaw,DeltaTime,RotationInterpSpeed);
    CurrentPitch = FMath::FInterpTo(CurrentPitch,TargetPitch,DeltaTime,RotationInterpSpeed);

    if (UTurretAnimInstance* Anim =Cast<UTurretAnimInstance>(TurretMesh->GetAnimInstance()))
    {
        Anim->TargetYaw = CurrentYaw;
        Anim->TargetPitch = CurrentPitch;
    }
}

void AAntiAirTurret::OnEnterSphere(UPrimitiveComponent*,AActor* OtherActor,UPrimitiveComponent*,int32,bool,const FHitResult&)
{

    if (bDestroyed)
        return;
    
    if (ADronePawn* Drone = Cast<ADronePawn>(OtherActor))
    {
        TargetDrone = Drone;
        Drone->SetUnderTurretFire(true);
        StartFiring(); 
      
    }
}

void AAntiAirTurret::OnExitSphere(UPrimitiveComponent*,AActor* OtherActor,UPrimitiveComponent*,int32)
{
    if (OtherActor == TargetDrone)
    {

        if (TargetDrone)
        {
            TargetDrone->SetUnderTurretFire(false);  
        }

        TargetDrone = nullptr;
        StopFiring();
   
    }
}

void AAntiAirTurret::StartFiring()
{
    if (!ProjectileClass)
        return;

    if (MuzzleFlashComponent && MuzzleFlashSystem)
    {
        MuzzleFlashComponent->SetAsset(MuzzleFlashSystem);
        MuzzleFlashComponent->Activate(true);
    }

    GetWorld()->GetTimerManager().SetTimer(
        FireTimer,
        this,
        &AAntiAirTurret::Fire,
        FireInterval,
        true
    );

    if (FireAudio && FireLoopSound)
    {
        FireAudio->SetSound(FireLoopSound);
        FireAudio->Play();
    }
}

void AAntiAirTurret::StopFiring()
{
    GetWorld()->GetTimerManager().ClearTimer(FireTimer);

    if (MuzzleFlashComponent)
    {
        MuzzleFlashComponent->Deactivate();
    }

    if (FireAudio && FireAudio->IsPlaying())
    {
        FireAudio->Stop();
    }
}
void AAntiAirTurret::Fire()
{
    
    
    if (!TargetDrone || !ProjectileClass)
        return;

    const FVector Muzzle =TurretMesh->GetSocketLocation("Muzzle");
    const FVector Target =TargetDrone->Mesh->GetSocketLocation("Drone_Target");
    const FVector Direction =(Target - Muzzle).GetSafeNormal();
    const FRotator SpawnRotation =Direction.Rotation();

    FActorSpawnParameters Params;
    Params.Owner = this;

    GetWorld()->SpawnActor<ATurretProjectile>(ProjectileClass,Muzzle,SpawnRotation,Params);
}

void AAntiAirTurret::DestroyByArtilleryStrike()
{
    DestroyTurret_Internal();
}

void AAntiAirTurret::DestroyTurret_Internal()
{
    if (bDestroyed)
        return;

    bDestroyed = true;

    OnTurretDestroyedByArtillery.Broadcast();
    
    ADronePawn* PrevDrone = TargetDrone;
    TargetDrone = nullptr;

    StopFiring();

    if (PrevDrone && IsValid(PrevDrone))
    {
        PrevDrone->SetUnderTurretFire(false);
    }

    if (DetectionSphere)
    {
        DetectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        DetectionSphere->SetGenerateOverlapEvents(false);
    }

  
    if (TurretMesh)
    {
        TurretMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    const FVector Location = GetActorLocation();

    if (ExplosionNiagara)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ExplosionNiagara,Location,GetActorRotation(),ExplosionScale);
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this,ExplosionSound,Location,4.0f,     0.95f      );
    }

    if (TurretMesh && DestroyedMaterial)
    {
        const int32 MatCount = TurretMesh->GetNumMaterials();

        for (int32 i = 0; i < MatCount; ++i)
        {
            TurretMesh->SetMaterial(i, DestroyedMaterial);
        }
    }
}

void AAntiAirTurret::ResetTurret()
{
    bDestroyed = false;

    if (TurretMesh && OriginalMaterials.Num() > 0)
    {
        for (int32 i = 0; i < OriginalMaterials.Num(); ++i)
        {
            TurretMesh->SetMaterial(i, OriginalMaterials[i]);
        }
    }

    if (DetectionSphere)
    {
        DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        DetectionSphere->SetGenerateOverlapEvents(true);
    }

    if (TurretMesh)
    {
        TurretMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    TargetDrone = nullptr;

  
    ADronePawn* Drone = Cast<ADronePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

    if (Drone)
    {
        float Dist = FVector::Dist(
            Drone->GetActorLocation(),
            GetActorLocation()
        );

        if (Dist <= DetectionRadius)
        {
            TargetDrone = Drone;
            Drone->SetUnderTurretFire(true);
            StartFiring();
        }
    }


}
