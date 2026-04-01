#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "ATutorialManager.generated.h"

UCLASS()
class DRONEPROJECT_API ATutorialManager : public AActor
{
    GENERATED_BODY()

public:
    ATutorialManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // ===== Tutorial Flags =====

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Movement")
    bool bMoveForwardBackwardCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Movement")
    bool bMoveLeftRightCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Rotation")
    bool bRotateLeftRightCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Vertical")
    bool bMoveUpCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Vertical")
    bool bMoveDownCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Actions")
    bool bTakeoffCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Actions")
    bool bChangeModeCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Camera")
    bool bCameraToggleCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Camera")
    bool bCameraArrowsCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial")
    bool bTutorialCompleted = false;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Actions")
    bool bRightMouseCompleted = false;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTutorialFinished);

    UPROPERTY(BlueprintAssignable, Category="Tutorial")
    FTutorialFinished OnTutorialFinished;

    UPROPERTY(BlueprintReadOnly, Category="Tutorial|Actions")
    bool bLeftMouseCompleted = false;

    

    // ===== Sounds =====

    UPROPERTY(EditAnywhere, Category="Tutorial|Sound")
    USoundBase* StepCompletedSound;

    UPROPERTY(EditAnywhere, Category="Tutorial|Sound")
    USoundBase* TutorialFinishedSound;

    UPROPERTY(EditAnywhere, Category="Tutorial|Sound")
    USoundBase* TeachSound2;

    FTimerHandle TeachSound2Timer;

private:

    void PlayStepSound();
    void PlayTutorialFinishedSound();
    void CheckTutorialCompleted();
    void PlayTeachSound2();

    // Timers
    float WHoldTime = 0.f;
    float SHoldTime = 0.f;
    float AHoldTime = 0.f;
    float DHoldTime = 0.f;
    float QHoldTime = 0.f;
    float EHoldTime = 0.f;
    float ShiftHoldTime = 0.f;
    float CtrlHoldTime = 0.f;
    float ArrowUpHoldTime = 0.f;
    float ArrowDownHoldTime = 0.f;
    float ArrowLeftHoldTime = 0.f;
    float ArrowRightHoldTime = 0.f;
    float LeftMouseHoldTime = 0.f;
    float RightMouseHoldTime = 0.f;
    // Held flags
    bool bWHeld = false;
    bool bSHeld = false;
    bool bAHeld = false;
    bool bDHeld = false;
    bool bQHeld = false;
    bool bEHeld = false;
    bool bArrowUpHeld = false;
    bool bArrowDownHeld = false;
    bool bArrowLeftHeld = false;
    bool bArrowRightHeld = false;
    bool bLeftMouseHeld = false;
    bool bRightMouseHeld = false;

    const float RequiredHoldTime = 0.7f;
};