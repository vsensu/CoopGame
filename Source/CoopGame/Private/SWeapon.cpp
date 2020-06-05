// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "CoopGame/CoopGame.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVarDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing,
                                               TEXT("Draw Debug Liens for Weapons"), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.0f;

	// Bullet per minute
	RateOfFire = 600;

	SetReplicates(true);
	NetUpdateFrequency = 60.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ASWeapon::Fire()
{
	if(GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}
	
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
		QueryParams.bReturnPhysicalMaterial = true;

		EPhysicalSurface surfaceType = EPhysicalSurface::SurfaceType_Default;
		
		if(GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, LineTraceEnd, COLLISION_WEAPON, QueryParams))
		{
			auto *HitActor = Hit.GetActor();

			surfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			
			auto actualDamage = BaseDamage;
			if(surfaceType == SURFACE_FLESHVULNERABLE)
			{
				actualDamage *= 4;
			}
			UGameplayStatics::ApplyPointDamage(HitActor, actualDamage, ShotDirection, Hit, owner->GetInstigatorController(), this, DamageType);
			
			PlayImpactEffects(surfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;
		}

		if(DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, LineTraceEnd, FColor::White, false, 1, 0, 1);
		}

		PlayFireEffects(TracerEndPoint);

		if(GetLocalRole() == ROLE_Authority)
		{
			HitScanTrace.SurfaceType = surfaceType;
			HitScanTrace.TraceTo = TracerEndPoint;
		}

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic FX
	PlayFireEffects(HitScanTrace.TraceTo);

	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::StartFire()
{
	float firstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, firstDelay);
}

void ASWeapon::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void ASWeapon::PlayFireEffects(const FVector &TracerEndPoint)
{
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

	auto *owner = Cast<APawn>(GetOwner());
	if(owner)
	{
		auto *pc = Cast<APlayerController>(owner->GetController());
		if(pc)
		{
			pc->ClientPlayCameraShake(FireCamShake);
		}
	}
}

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, const FVector& ImpactPoint)
{
	UParticleSystem *selectedImpactEffect = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
    case SURFACE_FLESHVULNERABLE:
        selectedImpactEffect = FleshImpactEffect;
		break;
	default:
        selectedImpactEffect = DefaultImpactEffect;
		break;
	}

	if (selectedImpactEffect)
	{
		auto MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		auto ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), selectedImpactEffect, ImpactPoint,
                                                 ShotDirection.Rotation());
	}
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}
