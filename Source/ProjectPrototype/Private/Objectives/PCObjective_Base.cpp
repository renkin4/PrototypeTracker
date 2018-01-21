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

void APCObjective_Base::OnAllCriticalMissionSucceeded()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("ALL Critical Missions Have Been Completed"));
}

void APCObjective_Base::SaveAllTrackerComponent()
{
	// save all the Components in the AllTrackerComponent Variable
	GetComponents<UPCObjectiveTracker_Base>(AllTrackerComponents);
	// check how many Critical Components are there for failure and success
	for (UPCObjectiveTracker_Base* ObjTrackerComp : AllTrackerComponents) 
	{
		if (ObjTrackerComp->GetIsCriticalTracker() && ObjTrackerComp->GetTrackerBranch().bSuccessBranch)
				++AmountOfCriticalTrackerSuccess;
	}	

}

void APCObjective_Base::ForceSuccess()
{
	SetObjectiveState(EState::Succeed);

	for (UPCObjectiveTracker_Base* ObjTracker : AllTrackerComponents) 
	{
		EState TrackerState = ObjTracker->GetTrackerState();
		//Fail all uncompleted tracker
		switch (TrackerState)
		{
		case EState::Idle:
			ObjTracker->SetTrackerState(EState::Failed);
			break;
		case EState::Activated:
			ObjTracker->SetTrackerState(EState::Failed);
			break;
		}
	}

}

void APCObjective_Base::ForceFail()
{
	SetObjectiveState(EState::Failed);
}

void APCObjective_Base::RunCheckList()
{
	// TODO Check all the Trackers
	for (UPCObjectiveTracker_Base* ObjTrackerComp : AllTrackerComponents)
	{
		int32 CriticalSuccessAmount = 0;

		// if complete and save it to a completed holder
		if (ObjTrackerComp->GetTrackerState() == EState::Succeed) 
		{
			SucceededTrackerHolder.Add(ObjTrackerComp);
			if (ObjTrackerComp->GetIsCriticalTracker())
				++CriticalSuccessAmount;
		}
			
		if (ObjTrackerComp->GetTrackerState() == EState::Failed)
			FailedTrackerHolder.Add(ObjTrackerComp);

		//Check if all Critical Mission are completed
		if (CriticalSuccessAmount >= AmountOfCriticalTrackerSuccess)
			OnAllCriticalMissionSucceeded();
	}

	// if all necessary objective Completed, Then Send Delegates and Inform Players
	// have a tracker to Force Success the objective actor
	// refresh UI
	RefreshUI();
}

void APCObjective_Base::ActivateTracker(int32 TrackerIndex)
{
	UPCObjectiveTracker_Base* ObjTracker = AllTrackerComponents[TrackerIndex];
	if (ObjTracker->IsValidLowLevel())
		ObjTracker->RunState();
}

UPCObjectiveTracker_Base* APCObjective_Base::GetTracker(int32 TrackerIndex)
{
	if(AllTrackerComponents[TrackerIndex]->IsValidLowLevel())
		return AllTrackerComponents[TrackerIndex];

	return NULL;
}

void APCObjective_Base::SetObjectiveState(EState NewState)
{
	ObjectiveState = NewState;
	RefreshUI();

	if(GetObjectiveState() == EState::Succeed)
		SucceededEvent();
	if(GetObjectiveState() == EState::Failed)
		FailedEvent();
}

