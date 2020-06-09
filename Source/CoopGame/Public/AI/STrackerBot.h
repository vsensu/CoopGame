// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent *MeshComp;

	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	bool bUseAccelChange;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	class USHealthComponent *HealthComp;

	UFUNCTION()
    void OnHealthChanged(USHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UMaterialInstanceDynamic *MatInst;

	void Explode();

	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	UParticleSystem *ExplodeEffect;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	float ExplodeDamage;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	float DamageRadius;

	UPROPERTY(EditDefaultsOnly, Category="Components")
	class USphereComponent *SphereComp;

	bool bStartedSelfDestruction;
	
	FTimerHandle TimerHandle_SelfDamage;

	UFUNCTION()
	void DamageSelf();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
