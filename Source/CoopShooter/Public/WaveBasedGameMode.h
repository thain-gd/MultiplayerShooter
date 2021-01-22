#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WaveBasedGameMode.generated.h"

enum class EWaveState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

UCLASS()
class COOPSHOOTER_API AWaveBasedGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWaveBasedGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
	
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnBot();

private:
	void SpawnBotTimerElapsed();

	void PrepareForNextWave();
	void StartWave();
	void EndWave();

	void CheckWaveState();
	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	
private:
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	FTimerHandle BotSpawnerTimerHandle;
	FTimerHandle NextWaveStartTimerHandle;
	uint32 RemainedBotsToSpawn;
	uint32 WaveCount;
};
