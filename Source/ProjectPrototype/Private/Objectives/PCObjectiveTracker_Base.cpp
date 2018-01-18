// Fill out your copyright notice in the Description page of Project Settings.

#include "Objectives/PCObjectiveTracker_Base.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UPCObjectiveTracker_Base::UPCObjectiveTracker_Base(const class FObjectInitializer & PCIP)
	:Super(PCIP)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	CurrentTrackState = ETrackerState::Idle;
	CompletedBranches = 0;
	// ...
}


// Called when the game starts
void UPCObjectiveTracker_Base::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UPCObjectiveTracker_Base::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//TODO set it to repNotify for these
	if (CurrentTrackState == ETrackerState::Activated)
	{
		for (FTrackerBranch Branches : TrackerBranches) 
		{
			Branches.RunBranch();
			//Check if the branch is completed
			if (Branches.bCompleted) 
			{
				++CompletedBranches;
				//If any Failed Branch manage to get completed, Force Break loop and complete Event with Fail State
				if (!Branches.bSuccessBranch) 
				{
					SetTrackerState(ETrackerState::Failed);
					break;
				}
			}
		}
		//Check if all objectives complete else recheck
		if (CompletedBranches == TrackerBranches.Num() && GetTrackerState() != ETrackerState::Failed)
			SetTrackerState(ETrackerState::Succeed);
		else
			CompletedBranches = 0;
	}
}

void UPCObjectiveTracker_Base::SucceededEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Tracker Objective Succeeded"));
	UE_LOG(LogTemp, Warning, TEXT("Tracker Objective Succeeded"));
}

void UPCObjectiveTracker_Base::FailedEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Tracker Objective Failed"));
	UE_LOG(LogTemp, Warning, TEXT("Tracker Objective Failed"));
}

void UPCObjectiveTracker_Base::SetTrackerState(ETrackerState SetState)
{
	CurrentTrackState = SetState;

	if (CurrentTrackState == ETrackerState::Succeed)
		SucceededEvent();
	if (CurrentTrackState == ETrackerState::Failed)
		FailedEvent();
}
