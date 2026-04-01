// Fill out your copyright notice in the Description page of Project Settings.


#include "DPGameMode.h"
#include "DroneProject/Pawn/DronePawn.h"
#include "DroneProject/Pawn/DroneController.h"

ADPGameMode::ADPGameMode()


{

   DefaultPawnClass = ADronePawn::StaticClass();
   PlayerControllerClass = ADroneController::StaticClass();
   
}



void ADPGameMode::BeginPlay()
{
  Super::BeginPlay();

  
 }