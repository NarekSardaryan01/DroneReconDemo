// Fill out your copyright notice in the Description page of Project Settings.



#include "DroneController.h"
#include "DronePawn.h"
#include "Blueprint/UserWidget.h"


void ADroneController::SetupInputComponent()

{

    Super::SetupInputComponent();


    InputComponent->BindAxis("MoveForward", this, &ADroneController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &ADroneController::MoveRight);
    InputComponent->BindAxis("MoveUp", this, &ADroneController::MoveUp);
    InputComponent->BindAxis("TurnYaw", this, &ADroneController::TurnYaw);
    InputComponent->BindAction("StartEngine", IE_Pressed, this, &ADroneController::StartEngine);
    InputComponent->BindAxis("FPV_LookUp", this, &ADroneController::FPVLookUp);
    InputComponent->BindAxis("FPV_Turn", this, &ADroneController::FPVTurn);
    InputComponent->BindAction("ToggleFPV",IE_Pressed,this,&ADroneController::ToggleFPV);
    InputComponent->BindAction("DriveMode",IE_Pressed,this,&ADroneController::CycleFlightMode);
    InputComponent->BindAxis("LookYaw", this, &ADroneController::LookYaw);
    InputComponent->BindAxis("LookPitch", this, &ADroneController::LookPitch);
    InputComponent->BindAction("ReconScan", IE_Pressed, this, &ADroneController::StartReconScan);
    InputComponent->BindAction("ReconScan",IE_Released,this,&ADroneController::StopReconScan);
    InputComponent->BindAction("ArtilleryDesignate", IE_Pressed, this, &ADroneController::StartArtilleryDesignate);
    InputComponent->BindAction("ArtilleryDesignate", IE_Released, this, &ADroneController::StopArtilleryDesignate);


  
}


void ADroneController::BeginPlay()
{
 
    Super::BeginPlay();

    if (DroneHUDClass)
    {
        CreateWidget<UUserWidget>(this, DroneHUDClass)->AddToViewport();
    }
    
}

void ADroneController::MoveForward(float Value)

{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetMoveForward(Value);
    }
    
}

void ADroneController::MoveRight(float Value)
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetMoveRight(Value);
    }

    
}


void ADroneController::MoveUp(float Value)


{

    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetMoveUp(Value);
    }
    
}

void ADroneController::TurnYaw(float Value)

{
   if ( ADronePawn* Drone = Cast<ADronePawn>(GetPawn()) )
   {
       Drone->SetYawInput(Value);
   }
    
}

void ADroneController::StartEngine()
{

    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        
        Drone->SetStartEngine();
        
    }
    
}

void ADroneController::FPVLookUp(float Value)
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetFPVLookUp(Value);
    }
}

void ADroneController::FPVTurn(float Value)
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetFPVTurn(Value);
    }
}

void ADroneController::ToggleFPV()
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetToggleFPV();
    }
}



void ADroneController::CycleFlightMode()
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetCycleFlightMode();
    }
}

void ADroneController::LookYaw(float V)
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetCameraLookYaw(V);
    }
}

void ADroneController::LookPitch(float V)
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetCameraLookPitch(V);
    }
}

void ADroneController::StartReconScan()
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->StartReconScan();
    }
}

void ADroneController::StopReconScan()
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->StopReconScan();
    }
}
void ADroneController::StartArtilleryDesignate()
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetStartArtilleryDesignate();
    }
}

void ADroneController::StopArtilleryDesignate()
{
    if (ADronePawn* Drone = Cast<ADronePawn>(GetPawn()))
    {
        Drone->SetStopArtilleryDesignate();
    }
}