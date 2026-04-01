#include "ArtilleryShell.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DroneProject/AntiDroneSystems/AntiAirTurret.h"

AArtilleryShell::AArtilleryShell()
{
  PrimaryActorTick.bCanEverTick = true;

  Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  SetRootComponent(Root);


  Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
  Collision->SetupAttachment(Root);


  Collision->SetSphereRadius(30.f);
  Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  Collision->SetCollisionObjectType(ECC_WorldDynamic);
  Collision->SetCollisionResponseToAllChannels(ECR_Block);
  Collision->SetGenerateOverlapEvents(false);
  Collision->SetNotifyRigidBodyCollision(true);

  FallAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FallAudioComponent"));
  FallAudioComponent->SetupAttachment(Root);
  FallAudioComponent->bAutoActivate = false; 
  FallAudioComponent->bStopWhenOwnerDestroyed = true;


  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetupAttachment(Collision);
  Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


  Collision->OnComponentHit.AddDynamic(this, &AArtilleryShell::OnHit);
}

void AArtilleryShell::BeginPlay()
{
  Super::BeginPlay();

  if (FallingSound && FallAudioComponent)
  {
    FallAudioComponent->SetSound(FallingSound);
    FallAudioComponent->Play();
  }
}

void AArtilleryShell::Tick(float DeltaTime)

{
  Super::Tick(DeltaTime);

  if (ImpactPoint.IsZero())return;

  FVector CurrentLocation = GetActorLocation();
  FVector Direction = (ImpactPoint - CurrentLocation).GetSafeNormal();
  FVector NewLocation = CurrentLocation + Direction * FallSpeed * DeltaTime;
  

  float DistanceToImpact = FVector::Dist(NewLocation, ImpactPoint);


  if (DistanceToImpact < 100.f)
  {
    if (TargetTurret && IsValid(TargetTurret))
    {
      TargetTurret->DestroyByArtilleryStrike();
    }

    if (ExplosionFX)
    {
      UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ExplosionFX,ImpactPoint);
    }

    if (ExplosionSound)
    {
      UGameplayStatics::PlaySoundAtLocation(this,ExplosionSound,ImpactPoint
      );
    }

    if (FallAudioComponent && FallAudioComponent->IsPlaying())
    {
      FallAudioComponent->Stop(); // резкий обрыв
    }
    
    Destroy();
    return;
  }

  SetActorLocation(NewLocation);
}
void AArtilleryShell::OnHit(UPrimitiveComponent*,AActor* OtherActor,UPrimitiveComponent*,FVector,const FHitResult&)
{

  if (FallAudioComponent && FallAudioComponent->IsPlaying())
  {
    FallAudioComponent->Stop();
  }
  
  if (AAntiAirTurret* Turret = Cast<AAntiAirTurret>(OtherActor))
  {
    Turret->DestroyByArtilleryStrike();
  }

  if (ExplosionFX)
  {
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ExplosionFX,GetActorLocation());
  }

  if (ExplosionSound)
  {
    UGameplayStatics::PlaySoundAtLocation(this,ExplosionSound,GetActorLocation());
  }

  Destroy();
}

void AArtilleryShell::SetTargetTurret(AAntiAirTurret* InTurret)
{
  TargetTurret = InTurret;
}

void AArtilleryShell::SetImpactPoint(const FVector& InPoint)
{
  ImpactPoint = InPoint;
}