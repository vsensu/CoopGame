// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVarDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing,
                                               TEXT("Draw Debug Liens for Weapons"), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Fire()
{
	auto *owner = GetOwner();
	if(owner)
	{
		FHitResult Hit;

		FVector EyeLocation;
		FRotator EyeRotation;
		owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		auto ShotDirection = EyeRotation.Vector();
		
		auto LineTraceEnd = EyeLocation + (ShotDirection * 10000);

		// tracer effect parameter
		auto TracerEndPoint = LineTraceEnd;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(owner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		
		if(GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, LineTraceEnd, ECC_Visibility, QueryParams))
		{
			auto *HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, owner->GetInstigatorController(), this, DamageType);

			if(ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		if(DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, LineTraceEnd, FColor::White, false, 1, 0, 1);
		}

		if(MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}

		if(TracerEffect)
		{
			auto MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			
			auto *TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
			if(TracerComp)
			{
				TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
			}
		}
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

