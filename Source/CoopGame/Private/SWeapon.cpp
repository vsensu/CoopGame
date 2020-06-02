// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"

#include "DrawDebugHelpers.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

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

		auto LineTraceEnd = EyeLocation + (EyeRotation.Vector() * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(owner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		
		if(GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, LineTraceEnd, ECC_Visibility, QueryParams))
		{
			
		}

		DrawDebugLine(GetWorld(), EyeLocation, LineTraceEnd, FColor::White, false, 1, 0, 1);
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

