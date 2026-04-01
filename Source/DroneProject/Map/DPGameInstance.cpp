#include "DPGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void UDPGameInstance::OnStart()
{
    Super::OnStart();
    

    if (AmbientSound && !AmbientAudio)
    {
        AmbientAudio = UGameplayStatics::SpawnSound2D(this,AmbientSound,1.0f,   1.f,0.f, nullptr,true);

        if (AmbientAudio)
        {
            AmbientAudio->Play();
                              
        }
    }

  
    if (WindSound && !WindAudio)
    {
        WindAudio = UGameplayStatics::SpawnSound2D(this,WindSound,1.0f,          WindMinPitch,0.f,nullptr,true);

        if (WindAudio)
        {
            WindAudio->SetVolumeMultiplier(0.f); 
            WindAudio->Play();

        }
    }

    if (TeachSound1)
    {
        FTimerHandle LocalHandle;

        FTimerDelegate Delegate;
        Delegate.BindLambda([this]()
        {
            UGameplayStatics::PlaySound2D(this,TeachSound1,2.0f  );
        });

        GetWorld()->GetTimerManager().SetTimer(LocalHandle,Delegate,3.0f,false);
    }
}

void UDPGameInstance::SetDroneSpeed01(float Speed01)
{
    DroneSpeed01 = FMath::Clamp(Speed01, 0.f, 1.f);

    if (!WindAudio)return;
    
    if (DroneSpeed01 < WindStartThreshold)
    {
        WindAudio->SetVolumeMultiplier(0.f);
        return;
    }

    const float Volume = FMath::GetMappedRangeValueClamped(FVector2D(WindStartThreshold, 1.f),FVector2D(0.f, WindMaxVolume),DroneSpeed01);

    const float Pitch = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f),FVector2D(WindMinPitch, WindMaxPitch),DroneSpeed01);

    WindAudio->SetVolumeMultiplier(Volume);
    WindAudio->SetPitchMultiplier(Pitch);
}