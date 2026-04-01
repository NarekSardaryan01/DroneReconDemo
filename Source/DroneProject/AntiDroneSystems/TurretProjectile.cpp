#include "TurretProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "DroneProject/Pawn/DronePawn.h"

ATurretProjectile::ATurretProjectile()
{
  PrimaryActorTick.bCanEverTick = false;

  Collision = CreateDefaultSubobject<USphereComponent>("Collision");
  SetRootComponent(Collision);

  Collision->InitSphereRadius(5.f);
  Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  Collision->SetCollisionResponseToAllChannels(ECR_Block);
  Collision->SetNotifyRigidBodyCollision(true);

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
  Mesh->SetupAttachment(Collision);
  Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  Movement = CreateDefaultSubobject<UProjectileMovementComponent>("Movement");
  Movement->InitialSpeed = 85000.f;
  Movement->MaxSpeed = 85000.f;
  Movement->bRotationFollowsVelocity = true;
  Movement->ProjectileGravityScale = 0.f;

    TrailComponent = CreateDefaultSubobject<UNiagaraComponent>("TrailComponent");
    TrailComponent->SetupAttachment(RootComponent);
    TrailComponent->SetAutoActivate(false);


  InitialLifeSpan = 6.f;

  Collision->OnComponentHit.AddDynamic(this, &ATurretProjectile::OnHit);
}

void ATurretProjectile::BeginPlay()
{
  Super::BeginPlay();

  if (TrailNiagara && TrailComponent)
  {
    TrailComponent->SetAsset(TrailNiagara);
    TrailComponent->SetWorldScale3D(TrailNiagaraScale);
    TrailComponent->Activate(true);
  }
}

void ATurretProjectile::OnHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit)
{
  if (ADronePawn* Drone = Cast<ADronePawn>(OtherActor))
  {
  
    Drone->ApplyDamage(Damage);

    
    if (ImpactSound)
    {
      UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,Hit.ImpactPoint,ImpactSoundVolume,1.0f);
    }

   
    if (ImpactNiagara)
    {
      UNiagaraComponent* NiagaraComp =UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ImpactNiagara,Hit.ImpactPoint,Hit.ImpactNormal.Rotation(),ImpactNiagaraScale );

      if (NiagaraComp)
      {
        NiagaraComp->SetAutoDestroy(true);
      }
    }
  }

  Destroy();
}