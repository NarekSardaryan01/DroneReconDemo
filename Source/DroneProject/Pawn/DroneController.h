// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DroneController.generated.h"

UCLASS()
class DRONEPROJECT_API ADroneController : public APlayerController {
  GENERATED_BODY()


protected:

  virtual void BeginPlay() override;
  
  
  virtual void SetupInputComponent() override;

      void MoveForward(float Value);
      void MoveRight(float Value);
      void MoveUp(float Value);
      void TurnYaw(float Value);
      void StartEngine();
      void FPVLookUp(float Value);
      void FPVTurn(float Value);
      void ToggleFPV();
      void CycleFlightMode();
      void LookYaw(float Value);
      void LookPitch(float Value);
      void StartReconScan();
      void StopReconScan();
       void StartArtilleryDesignate();
      void StopArtilleryDesignate();
   
public:
  
  UPROPERTY(EditDefaultsOnly, Category="UI")
  TSubclassOf<UUserWidget> DroneHUDClass;

  
  
};
