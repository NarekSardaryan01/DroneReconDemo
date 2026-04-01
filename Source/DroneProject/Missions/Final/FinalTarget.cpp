#include "FinalTarget.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DroneProject/Pawn/DronePawn.h"

AFinalTarget::AFinalTarget()
{
    PrimaryActorTick.bCanEverTick = false;

    FinishCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FinishCollision"));
    RootComponent = FinishCollision;

    FinishCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    FinishCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    FinishCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    VisualCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualCylinder"));
    VisualCylinder->SetupAttachment(RootComponent);
    VisualCylinder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFinalTarget::BeginPlay()
{
    Super::BeginPlay();

    if (FinishCollision)
    {
        FinishCollision->OnComponentBeginOverlap.AddDynamic(this, &AFinalTarget::OnBeginOverlap);
    }

    DeactivateTarget();
}

void AFinalTarget::ActivateTarget()
{
    bActive = true; 
    bTriggered = false;

    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
}

void AFinalTarget::DeactivateTarget()
{
    bActive = false;
    bTriggered = false;

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}

void AFinalTarget::OnBeginOverlap( UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
    if (!bActive || bTriggered) return;

    if (Cast<ADronePawn>(OtherActor))
    {
        bTriggered = true;              
        OnTargetTriggered.Broadcast(this); 
    }
}