#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TurretAnimInstance.generated.h"

UCLASS()
class DRONEPROJECT_API UTurretAnimInstance : public UAnimInstance
{
  GENERATED_BODY()

public:

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turret")
  float TargetYaw = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turret")
  float TargetPitch = 0.f;
};