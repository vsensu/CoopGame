// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"

#include "Kismet/GameplayStatics.h"

void ASProjectileWeapon::Fire()
{
    auto *owner = GetOwner();
    if(owner && ProjectileClass)
    {
        FVector EyeLocation;
        FRotator EyeRotation;
        owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

        auto MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

        FActorSpawnParameters SpawnParameters;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
        GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParameters);
    }
}
