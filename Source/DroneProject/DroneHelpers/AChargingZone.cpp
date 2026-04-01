#include "AChargingZone.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DroneProject/Pawn/DronePawn.h"

AChargingZone::AChargingZone()
{
  PrimaryActorTick.bCanEverTick = true;

  Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
  SetRootComponent(Trigger);

  Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
  Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
  Trigger->SetGenerateOverlapEvents(true);

  Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
  Visual->SetupAttachment(Trigger);
  Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AChargingZone::BeginPlay()
{
  Super::BeginPlay();

  Trigger->OnComponentBeginOverlap.AddDynamic(this, &AChargingZone::OnOverlapBegin);
  Trigger->OnComponentEndOverlap.AddDynamic(this, &AChargingZone::OnOverlapEnd);
}

void AChargingZone::OnOverlapBegin(UPrimitiveComponent*,AActor* OtherActor,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
  ChargingDrone = Cast<ADronePawn>(OtherActor);
}


void AChargingZone::OnOverlapEnd(UPrimitiveComponent*,AActor* OtherActor,UPrimitiveComponent*,int32)
{
  if (OtherActor == Cast<AActor>(ChargingDrone))
  {
    ChargingDrone = nullptr;
  }
}


void AChargingZone::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (!ChargingDrone.IsValid())
    return;

  ChargingDrone->BatteryPercent = FMath::Clamp(ChargingDrone->BatteryPercent + ChargePerSecond * DeltaTime,0.f,100.f);
}