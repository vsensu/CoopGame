// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USHealthComponent *HealthComp;

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category="Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category="Player", meta=(ClampMin=0.0, ClampMax = 100))
	float ZoomSpeed;

	bool bWantsToZoom;

	void BeginZoom();

	void EndZoom();

	void StartFire();
	void StopFire();

	UPROPERTY(Replicated)
	class ASWeapon *CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category="Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category="Player")
	FName WeaponAttachSocketName;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category="Player")
	bool bDied;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector GetPawnViewLocation() const override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
