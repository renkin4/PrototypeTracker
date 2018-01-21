// Fill out your copyright notice in the Description page of Project Settings.

#include "Objectives/PCObjective_Base.h"
#include "Objectives/PCObjectiveTracker_Base.h"


// Sets default values
APCObjective_Base::APCObjective_Base(const class FObjectInitializer & PCIP)
	:Super(PCIP)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APCObjective_Base::BeginPlay()
{
	Super::BeginPlay();
	SaveAllTrackerComponent();
}

// Called every frame
void APCObjective_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APCObjective_Base::RefreshUI_Implementation()
{
	//TODO push data to UI and let Handle Refresh or Change Data
	OnRefreshUI.Broadcast();
}

void APCObjective_Base::SucceededEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Objective Succeeded"));
	OnObjSuccessDelegate.Broadcast();
}

void APCObjective_Base::FailedEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Objective Failed"));
	OnObjFailedDelegate.Broadcast();
}

void APCObjective_Base::SaveAllTrackerComponent()
{
	// save all the Components in the AllTrackerComponent Variable
	GetComponents<UPCObjectiveTracker_Base>(AllTrackerComponents);
}

void APCObjective_Base::RunState()
{
	SetObjectiveState(EState::Activated);
}

void APCObjective_Base::ForceSuccess()
{
	SetObjectiveState(EState::Succeed);
}

void APCObjective_Base::ForceFail()
{
	SetObjectiveState(EState::Failed);
}

void APCObjective_Base::ActivateTracker(int32 TrackerIndex)
{
	AllTrackerComponents[TrackerIndex]->Activate(true);
}

void APCObjective_Base::DeactivateTracker(int32 TrackerIndex)
{
	AllTrackerComponents[TrackerIndex]->Activate(false);
}

UPCObjectiveTracker_Base* APCObjective_Base::GetTracker(int32 TrackerIndex)
{
	return AllTrackerComponents[TrackerIndex]->IsValidLowLevel() ? AllTrackerComponents[TrackerIndex] : NULL;
}

void APCObjective_Base::SetObjectiveState(EState NewState)
{
	ObjectiveState = NewState;

	if(GetObjectiveState() == EState::Succeed)
		SucceededEvent();
	if(GetObjectiveState() == EState::Failed)
		FailedEvent();
}

