#include "Items/PowerupActor.h"
#include "Components/PointLightComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APowerupActor::APowerupActor()
{
	PowerupInterval = 0.0f;
	TotalNumberOfTicks = 0;

	bIsPowerupActive = false;

	SetReplicates(true);
}

void APowerupActor::ActivatePowerup(AActor* ActiveFor)
{
	OnActivated(ActiveFor);

	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (PowerupInterval > 0.0f)
		GetWorldTimerManager().SetTimer(PowerupTickTimerHandle, this, &APowerupActor::OnTickPowerup, PowerupInterval, true);
	else
		OnTickPowerup();
}

void APowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
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

void APowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APowerupActor, bIsPowerupActive);
}
