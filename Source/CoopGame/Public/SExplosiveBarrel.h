// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SExplosiveBarrel.generated.h"

class USHealthComponent;
class URadialForceComponent;

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent *MeshComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	USHealthComponent *HealthComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	URadialForceComponent *ForceComp;

	UFUNCTION()
    void OnHealthChanged(USHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category="Barrel")
	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category="Barrel")
	float ExplosionImpulse;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	UParticleSystem *ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	UMaterialInstance *ExplodedMaterial;

};
