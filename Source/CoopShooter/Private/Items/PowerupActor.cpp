#include "Items/PowerupActor.h"

// Sets default values
APowerupActor::APowerupActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	PowerupInterval = 0.0f;
	TotalNumberOfTicks = 0;
}

// Called when the game starts or when spawned
void APowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void APowerupActor::ActivatePowerup()
{
	OnActivated();

	if (PowerupInterval > 0.0f)
		GetWorldTimerManager().SetTimer(PowerupTickTimerHandle, this, &APowerupActor::OnTickPowerup, PowerupInterval, true);
	else
		OnTickPowerup();
}

void APowerupActor::OnTickPowerup()
{
	++TicksProcessed;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNumberOfTicks)
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(PowerupTickTimerHandle);
	}

}

