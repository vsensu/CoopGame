// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STrackerBot.h"



#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	RequiredDistanceToTarget = 100.0f;
	bUseAccelChange = false;
	MovementForce = 1000.0f;

}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	NextPathPoint = GetNextPathPoint();
}

FVector ASTrackerBot::GetNextPathPoint()
{
	auto *playerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	auto *path = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), playerPawn);

	if(path->PathPoints.Num() > 1)
	{
		return path->PathPoints[1];
	}

	return GetActorLocation();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto DistanceToTarget = FVector::Distance(GetActorLocation(), NextPathPoint);

	if(DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();
	}
	else
	{
		auto ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;

		MeshComp->AddForce(ForceDirection, NAME_None, bUseAccelChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), NextPathPoint, 32, FColor::Yellow, false, 0, 0, 1);
	}
	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0, 0, 1);
}

// Called to bind functionality to input
void ASTrackerBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

