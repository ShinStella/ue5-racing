// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CXGameState.generated.h"

class ACXPlayerState;

UENUM(BlueprintType)
enum class EGameState : uint8
{
	Waiting,
	Playing,
	Finished
};

UCLASS()
class CX_API ACXGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetGameState(EGameState NewGameState);
	void SetRemainingTime(int32 NewRemainingTime);

	int32 GetRemainingTime() { return RemainingTime; }

public:
	UPROPERTY(Replicated)
	EGameState CurrentGameState = EGameState::Waiting;

	UPROPERTY(Replicated)
	int32 RemainingTime = 0;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<ACXPlayerState>> Winners;
};
