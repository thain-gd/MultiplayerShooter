#include "WaveBasedGameMode.h"
#include "EngineUtils.h"
#include "Components/HealthComponent.h"

AWaveBasedGameMode::AWaveBasedGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	TimeBetweenWaves = 2.0f;
}

void AWaveBasedGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void AWaveBasedGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(NextWaveStartTimerHandle, this, &AWaveBasedGameMode::StartWave, TimeBetweenWaves, false);
}

void AWaveBasedGameMode::StartWave()
{
	++WaveCount;
	RemainedBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(BotSpawnerTimerHandle, this, &AWaveBasedGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
}

void AWaveBasedGameMode::SpawnBotTimerElapsed()
{
	SpawnBot();

	--RemainedBotsToSpawn;
	if (RemainedBotsToSpawn <= 0)
		EndWave();
}

void AWaveBasedGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(BotSpawnerTimerHandle);
}

void AWaveBasedGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
}

void AWaveBasedGameMode::CheckWaveState()
{
	bool bIsPreparingForNextWave = GetWorldTimerManager().IsTimerActive(NextWaveStartTimerHandle);
	if (RemainedBotsToSpawn > 0 || bIsPreparingForNextWave)
		return;

	bool bIsAnyBotAlive = false;
	for (TActorIterator<APawn> It = TActorIterator<APawn>(GetWorld()); It; ++It)
	{
		APawn* TestPawn = *It;
		if (!TestPawn || TestPawn->IsPlayerControlled())
			continue;

		UHealthComponent* HealthComp = Cast<UHealthComponent>(TestPawn->GetComponentByClass(UHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetCurrentHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive)
		PrepareForNextWave();
}
