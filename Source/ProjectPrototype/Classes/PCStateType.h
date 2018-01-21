// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PCStateType.generated.h"

UENUM(BlueprintType)
enum class EState : uint8
{
	// Tracker on Idle state will not run.
	Idle,

	// Tracker Run on Activated
	Activated,

	// On Succeeded Tracker will stop and pass info to Objective Actor
	Succeed,

	// On Failed Tracker will stop and pass info to Objective Actor
	Failed
};