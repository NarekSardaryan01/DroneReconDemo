#include "CheckpointActor.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DroneProject/Pawn/DronePawn.h"
#include "Kismet/GameplayStatics.h"

ACheckpointActor::ACheckpointActor()
{
  PrimaryActorTick.bCanEverTick = false;

  Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
  RootComponent = Sphere;

  Sphere->SetSphereRadius(260.f);
  Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  Sphere->OnComponentBeginOverlap.AddDynamic(this,&ACheckpointActor::OnSphereOverlap);

  VisualMesh = CreateDefaultSubobject<USkeletalMeshComponent>("VisualMesh");
  VisualMesh->SetupAttachment(RootComponent);
  VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACheckpointActor::BeginPlay()
{
  Super::BeginPlay();
  DeactivateCheckpoint();
}

void ACheckpointActor::ActivateCheckpoint()
{
  bIsActive = true;
  Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  VisualMesh->SetVisibility(true);
}

void ACheckpointActor::DeactivateCheckpoint()
{
  bIsActive = false;
  Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  VisualMesh->SetVisibility(false);
}

void ACheckpointActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
  if (!bIsActive) return;

  if (OtherActor && OtherActor->IsA(ADronePawn::StaticClass()))
  {
    DeactivateCheckpoint();

    if (CheckpointPassSound)
    {
      UGameplayStatics::PlaySound2D(this,CheckpointPassSound,CheckpointVolumeMultiplier);
    }

    OnCheckpointPassed.Broadcast(this);
  }
}