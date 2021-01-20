#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WaveBasedGameMode.generated.h"

UCLASS()
class COOPSHOOTER_API AWaveBasedGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWaveBasedGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnBot();

private:
	void SpawnBotTimerElapsed();

	void PrepareForNextWave();
	void StartWave();
	void EndWave();

	void CheckWaveState();

	
private:
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	FTimerHandle BotSpawnerTimerHandle;
	FTimerHandle NextWaveStartTimerHandle;
	uint32 RemainedBotsToSpawn;
	uint32 WaveCount;
};
