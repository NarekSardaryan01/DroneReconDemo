#include "AFixDroneZone.h"
#include "Components/BoxComponent.h"
#include "DroneProject/Pawn/DronePawn.h"


AFixDroneZone::AFixDroneZone()
{
  Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
  SetRootComponent(Trigger);

  Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
  Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

  Trigger->OnComponentBeginOverlap.AddDynamic(this, &AFixDroneZone::OnEnter);
  Trigger->OnComponentEndOverlap.AddDynamic(this, &AFixDroneZone::OnExit);

 
  VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
  VisualMesh->SetupAttachment(Trigger);

  VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  VisualMesh->SetGenerateOverlapEvents(false);
  VisualMesh->SetCastShadow(false);
}

void AFixDroneZone::OnEnter(UPrimitiveComponent*,AActor* OtherActor,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
  if (ADronePawn* Drone = Cast<ADronePawn>(OtherActor))
  {
    Drone->StartFixing(); 
  }
}

void AFixDroneZone::OnExit(UPrimitiveComponent*,AActor* OtherActor,UPrimitiveComponent*,int32)
{
  if (ADronePawn* Drone = Cast<ADronePawn>(OtherActor))
  {
    Drone->StopFixing();
  }
}