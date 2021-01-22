#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WaveGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,
	WaveInProgress,
	WaitingToComplete,
	WaveComplete,
	GameOver
};

/**
 * 
 */
UCLASS()
class COOPSHOOTER_API AWaveGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	void SetWaveState(EWaveState NewState);
	
protected:
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);


protected:
	UPROPERTY(ReplicatedUsing = OnRep_WaveState, BlueprintReadOnly, Category = "GameState")
	EWaveState WaveState;
	
};
