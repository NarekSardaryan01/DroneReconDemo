#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArtilleryTarget.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AAntiAirTurret;
class AArtilleryShell;
class UNiagaraSystem;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArtilleryTargetCompleted, AArtilleryTarget*, Target);

UCLASS()
class DRONEPROJECT_API AArtilleryTarget : public AActor
{
    GENERATED_BODY()

public:
    AArtilleryTarget();

  
    UPROPERTY(BlueprintAssignable)
    FOnArtilleryTargetCompleted OnTargetCompleted;

 
    UFUNCTION(BlueprintCallable, Category="Artillery")
    void ActivateTarget();

    UFUNCTION(BlueprintCallable, Category="Artillery")
    void DeactivateTarget();

 
    void ProcessDesignate(float DeltaTime, bool bIsLookedAt);

    UFUNCTION(BlueprintCallable, Category="Artillery")
    void SetLinkedTurret(AAntiAirTurret* InTurret);

    AAntiAirTurret* GetLinkedTurret() const { return LinkedTurret; }



protected:
    virtual void BeginPlay() override;


    UPROPERTY(VisibleAnywhere)
    UBoxComponent* DesignateCollision;


    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* VisualCylinder;

   
    UPROPERTY(EditAnywhere, Category="Target")
    AAntiAirTurret* LinkedTurret = nullptr;


    UPROPERTY(EditAnywhere, Category="Artillery")
    TSubclassOf<AArtilleryShell> ArtilleryShellClass;

    UPROPERTY(EditAnywhere, Category="Artillery")
    float ShellSpawnHeight = 18000.f;

    UPROPERTY(EditAnywhere, Category="Artillery")
    float RequiredHoldTime = 3.0f;


    UPROPERTY(EditAnywhere, Category="Feedback")
    UNiagaraSystem* MarkFX = nullptr;

    UPROPERTY(EditAnywhere, Category="Feedback")
    USoundBase* MarkSound = nullptr;

    UPROPERTY(EditAnywhere, Category="Feedback")
    float MarkSoundVolume = 1.0f;

private:
    float HoldTimer = 0.f;
    bool bActive = false;


    void ResetHold();
    void ConfirmStrike();
    void SpawnShell(const FVector& ImpactLocation);
};