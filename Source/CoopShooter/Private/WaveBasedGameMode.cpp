#include "WaveBasedGameMode.h"
#include "WaveGameState.h"
#include "EngineUtils.h"
#include "Components/HealthComponent.h"

AWaveBasedGameMode::AWaveBasedGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	GameStateClass = AWaveGameState::StaticClass();

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

	SetWaveState(EWaveState::WaitingToStart);
}

void AWaveBasedGameMode::StartWave()
{
	++WaveCount;
	RemainedBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(BotSpawnerTimerHandle, this, &AWaveBasedGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
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

	SetWaveState(EWaveState::WaitingToComplete);
}

void AWaveBasedGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
	CheckAnyPlayerAlive();
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
		if (HealthComp && HealthComp->IsAlive())
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive)
	{
		SetWaveState(EWaveState::WaveComplete);

		PrepareForNextWave();
	}
}

void AWaveBasedGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			UHealthComponent* HealthComp = Cast<UHealthComponent>(MyPawn->GetComponentByClass(UHealthComponent::StaticClass()));
			if (ensureAlways(HealthComp) && HealthComp->IsAlive())
			{
				return;
			}
		}
	}

	// No player alive
	GameOver();
}

void AWaveBasedGameMode::GameOver()
{
	EndWave();

	SetWaveState(EWaveState::GameOver);

	UE_LOG(LogTemp, Warning, TEXT("GAME OVER! Players Died"));
}

void AWaveBasedGameMode::SetWaveState(EWaveState NewState)
{
	AWaveGameState* GS = GetGameState<AWaveGameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}
