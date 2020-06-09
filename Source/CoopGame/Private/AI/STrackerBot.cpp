// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STrackerBot.h"



#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "SHealthComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(RequiredDistanceToTarget);
	SphereComp->SetupAttachment(MeshComp);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	RequiredDistanceToTarget = 100.0f;
	bUseAccelChange = false;
	MovementForce = 1000.0f;

	ExplodeDamage = 40.0f;
	DamageRadius = 200.0f;
	SelfDamageInterval = 0.25f;

}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::OnHealthChanged);

	if(GetLocalRole() == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();
	}
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

void ASTrackerBot::OnHealthChanged(USHealthComponent* InHealthComp, float Health, float HealthDelta,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if(MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if(MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
	
	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	if(Health <= 0.0f)
	{
		Explode();
	}
}

void ASTrackerBot::Explode()
{
	if(bExploded)
	{
		return;
	}

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplodeEffect, GetActorLocation());

	TArray<AActor*> ignoredActors;
	ignoredActors.Add(this);
	
	UGameplayStatics::ApplyRadialDamage(this, ExplodeDamage, GetActorLocation(), DamageRadius, nullptr
		, ignoredActors, this, GetInstigatorController(), true);

	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 12, FColor::Red, false, 2.0f, 0, 1);

	UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, GetActorLocation());
	
	Destroy();
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20.0f, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(GetLocalRole() == ROLE_Authority)
	{
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
}

// Called to bind functionality to input
void ASTrackerBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if(bStartedSelfDestruction)
	{
		return;
	}
	
	auto *playerPawn = Cast<ASCharacter>(OtherActor);
	if(playerPawn)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);

		bStartedSelfDestruction = true;

		UGameplayStatics::SpawnSoundAttached(ExplodeWarningSound, MeshComp);
	}
}

