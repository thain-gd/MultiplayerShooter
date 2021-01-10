// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerupActor.generated.h"

UCLASS()
class COOPSHOOTER_API APowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerupActor();

	void ActivatePowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnTickPowerup();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Powerups")
	UStaticMeshComponent* MeshComp;

	// Time between powerup ticks
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	// Total times we apply the powerup effect
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	uint32 TotalNumberOfTicks;

private:
	FTimerHandle PowerupTickTimerHandle;

	// Total number of ticks applied
	uint32 TicksProcessed;
};
