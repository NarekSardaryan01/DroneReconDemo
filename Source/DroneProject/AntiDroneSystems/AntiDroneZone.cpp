#include "AntiDroneZone.h"
#include "Components/BoxComponent.h"
#include "DroneProject/Pawn/DronePawn.h"

AAntiDroneZone::AAntiDroneZone()
{
  Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
  SetRootComponent(Trigger);

  Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
  Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

  Trigger->OnComponentBeginOverlap.AddDynamic(this, &AAntiDroneZone::OnEnter);
  Trigger->OnComponentEndOverlap.AddDynamic(this, &AAntiDroneZone::OnExit);
}

void AAntiDroneZone::OnEnter(UPrimitiveComponent*,AActor* OtherActor,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
  if (ADronePawn* Drone = Cast<ADronePawn>(OtherActor))
  {
    Drone->bInsideAntiDroneZone = true;
    Drone->bAntiDroneEverTriggered = true;
    Drone->bFixedOnce = false; 
  }
}

void AAntiDroneZone::OnExit(UPrimitiveComponent*,AActor* OtherActor,UPrimitiveComponent*,int32)
{
  if (ADronePawn* Drone = Cast<ADronePawn>(OtherActor))
  {
    Drone->bInsideAntiDroneZone = false;
  }
}