#include "ATutorialManager.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"

ATutorialManager::ATutorialManager()
{
    PrimaryActorTick.bCanEverTick = true;

    StepCompletedSound = nullptr;
    TutorialFinishedSound = nullptr;
}

void ATutorialManager::BeginPlay()
{
    Super::BeginPlay();
}

void ATutorialManager::PlayStepSound()
{
    if (StepCompletedSound)
    {
        UGameplayStatics::PlaySound2D(this, StepCompletedSound);
    }
}

void ATutorialManager::PlayTutorialFinishedSound()
{
    if (TutorialFinishedSound)
    {
        UGameplayStatics::PlaySound2D(this, TutorialFinishedSound);
    }
}

void ATutorialManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bTutorialCompleted)
        return;

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
        return;

    bool bAnyStepCompletedThisFrame = false;

    const float MoveForward = PC->GetInputAxisValue(TEXT("MoveForward"));
    const float MoveRight   = PC->GetInputAxisValue(TEXT("MoveRight"));
    const float TurnValue   = PC->GetInputAxisValue(TEXT("TurnYaw"));

    // ===== W + S =====
    if (!bWHeld && MoveForward > 0.1f)
    {
        WHoldTime += DeltaTime;
        if (WHoldTime >= RequiredHoldTime)
            bWHeld = true;
    }

    if (!bSHeld && MoveForward < -0.1f)
    {
        SHoldTime += DeltaTime;
        if (SHoldTime >= RequiredHoldTime)
            bSHeld = true;
    }

    if (bWHeld && bSHeld && !bMoveForwardBackwardCompleted)
    {
        bMoveForwardBackwardCompleted = true;
        bAnyStepCompletedThisFrame = true;
    }

    // ===== A + D =====
    if (!bAHeld && MoveRight < -0.1f)
    {
        AHoldTime += DeltaTime;
        if (AHoldTime >= RequiredHoldTime)
            bAHeld = true;
    }

    if (!bDHeld && MoveRight > 0.1f)
    {
        DHoldTime += DeltaTime;
        if (DHoldTime >= RequiredHoldTime)
            bDHeld = true;
    }

    if (bAHeld && bDHeld && !bMoveLeftRightCompleted)
    {
        bMoveLeftRightCompleted = true;
        bAnyStepCompletedThisFrame = true;
    }

    // ===== Q + E =====
    if (!bQHeld && TurnValue < -0.1f)
    {
        QHoldTime += DeltaTime;
        if (QHoldTime >= RequiredHoldTime)
            bQHeld = true;
    }

    if (!bEHeld && TurnValue > 0.1f)
    {
        EHoldTime += DeltaTime;
        if (EHoldTime >= RequiredHoldTime)
            bEHeld = true;
    }

    if (bQHeld && bEHeld && !bRotateLeftRightCompleted)
    {
        bRotateLeftRightCompleted = true;
        bAnyStepCompletedThisFrame = true;
    }

    // ===== SHIFT =====
    if (!bMoveUpCompleted &&
        (PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift)))
    {
        ShiftHoldTime += DeltaTime;
        if (ShiftHoldTime >= RequiredHoldTime)
        {
            bMoveUpCompleted = true;
            bAnyStepCompletedThisFrame = true;
        }
    }

    // ===== CTRL =====
    if (!bMoveDownCompleted &&
        (PC->IsInputKeyDown(EKeys::LeftControl) || PC->IsInputKeyDown(EKeys::RightControl)))
    {
        CtrlHoldTime += DeltaTime;
        if (CtrlHoldTime >= RequiredHoldTime)
        {
            bMoveDownCompleted = true;
            bAnyStepCompletedThisFrame = true;
        }
    }

    // ===== H =====
    if (PC->WasInputKeyJustPressed(EKeys::H) && !bTakeoffCompleted)
    {
        bTakeoffCompleted = true;
        bAnyStepCompletedThisFrame = true;
    }

    // ===== C =====
    if (PC->WasInputKeyJustPressed(EKeys::C) && !bCameraToggleCompleted)
    {
        bCameraToggleCompleted = true;
        bAnyStepCompletedThisFrame = true;
    }

    // ===== L =====
    if (PC->WasInputKeyJustPressed(EKeys::L) && !bChangeModeCompleted)
    {
        bChangeModeCompleted = true;
        bAnyStepCompletedThisFrame = true;
    }

    // ===== ARROWS =====
    if (!bArrowUpHeld && PC->IsInputKeyDown(EKeys::Up))
    {
        ArrowUpHoldTime += DeltaTime;
        if (ArrowUpHoldTime >= RequiredHoldTime)
            bArrowUpHeld = true;
    }

    if (!bArrowDownHeld && PC->IsInputKeyDown(EKeys::Down))
    {
        ArrowDownHoldTime += DeltaTime;
        if (ArrowDownHoldTime >= RequiredHoldTime)
            bArrowDownHeld = true;
    }

    if (!bArrowLeftHeld && PC->IsInputKeyDown(EKeys::Left))
    {
        ArrowLeftHoldTime += DeltaTime;
        if (ArrowLeftHoldTime >= RequiredHoldTime)
            bArrowLeftHeld = true;
    }

    if (!bArrowRightHeld && PC->IsInputKeyDown(EKeys::Right))
    {
        ArrowRightHoldTime += DeltaTime;
        if (ArrowRightHoldTime >= RequiredHoldTime)
            bArrowRightHeld = true;
    }

    if (bArrowUpHeld && bArrowDownHeld && bArrowLeftHeld && bArrowRightHeld && !bCameraArrowsCompleted)
    {
        bCameraArrowsCompleted = true;
        bAnyStepCompletedThisFrame = true;
    }

    // ===== LEFT MOUSE =====
    if (!bLeftMouseHeld)
    {
        if (PC->IsInputKeyDown(EKeys::LeftMouseButton))
        {
            LeftMouseHoldTime += DeltaTime;

            if (LeftMouseHoldTime >= RequiredHoldTime)
                bLeftMouseHeld = true;
        }
        else
        {
            LeftMouseHoldTime = 0.f;
        }
    }

    if (bLeftMouseHeld && !bLeftMouseCompleted)
    {
        bLeftMouseCompleted = true;
        bAnyStepCompletedThisFrame = true;
    }

    // ===== RIGHT MOUSE =====
    if (!bRightMouseHeld)
    {
        if (PC->IsInputKeyDown(EKeys::RightMouseButton))
        {
            RightMouseHoldTime += DeltaTime;

            if (RightMouseHoldTime >= RequiredHoldTime)
                bRightMouseHeld = true;
        }
        else
        {
            RightMouseHoldTime = 0.f;
        }
    }

    if (bRightMouseHeld && !bRightMouseCompleted)
    {
        bRightMouseCompleted = true;
        bAnyStepCompletedThisFrame = true;
    }

    CheckTutorialCompleted();

    if (bAnyStepCompletedThisFrame && !bTutorialCompleted)
    {
        PlayStepSound();
    }
}

void ATutorialManager::CheckTutorialCompleted()
{
    const bool bWasCompleted = bTutorialCompleted;

    bTutorialCompleted =
        bMoveForwardBackwardCompleted &&
        bMoveLeftRightCompleted &&
        bRotateLeftRightCompleted &&
        bMoveUpCompleted &&
        bMoveDownCompleted &&
        bTakeoffCompleted &&
        bCameraToggleCompleted &&
        bCameraArrowsCompleted &&
        bChangeModeCompleted &&
        bLeftMouseCompleted &&
        bRightMouseCompleted;

    if (!bWasCompleted && bTutorialCompleted)
    {
        PlayTutorialFinishedSound();
        OnTutorialFinished.Broadcast();

        if (TeachSound2)
        {
            GetWorld()->GetTimerManager().SetTimer(
                TeachSound2Timer,
                this,
                &ATutorialManager::PlayTeachSound2,
                1.0f,
                false
            );
        }
    }
}

void ATutorialManager::PlayTeachSound2()
{
    if (TeachSound2)
    {
        UGameplayStatics::PlaySound2D(this, TeachSound2, 2.0f);
    }
}