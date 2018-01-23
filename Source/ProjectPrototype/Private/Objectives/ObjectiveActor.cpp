// Copyright AmmoboxStudios Sdn Bhd 2009-2018

#include "Objectives/ObjectiveActor.h"
#include "Objectives/ObjectiveTrackerComponent.h"

//// Sets default values
AObjectiveActor::AObjectiveActor(const class FObjectInitializer & PCIP)
	:Super(PCIP)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}
//
void AObjectiveActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();

}

// Called when the game starts or when spawned
void AObjectiveActor::BeginPlay()
{
	Super::BeginPlay();
	SaveAllTrackerComponent();
	CheckIfActivated();
}

// Called every frame
void AObjectiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AObjectiveActor::RefreshUI_Implementation()
{
	//TODO push data to UI and let Handle Refresh or Change Data
	OnRefreshUI.Broadcast();
}

void AObjectiveActor::SucceededEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Objective Succeeded"));
	OnObjSuccessDelegate.Broadcast();
}

void AObjectiveActor::FailedEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Objective Failed"));
	OnObjFailedDelegate.Broadcast();
}

void AObjectiveActor::SaveAllTrackerComponent()
{
	// save all the Components in the AllTrackerComponent Variable
	GetComponents<UObjectiveTrackerComponent>(AllTrackerComponents);
}

void AObjectiveActor::CheckIfActivated()
{
	if (GetObjectiveState() == EState::Activated)
	{
		for (UObjectiveTrackerComponent* Trackers : GetAllTracker())
		{
			Trackers->SetComponentTickEnabled(true);
		}
	}
}

void AObjectiveActor::RunState()
{
	SetObjectiveState(EState::Activated);
}

void AObjectiveActor::ForceSuccess()
{
	SetObjectiveState(EState::Succeed);
}

void AObjectiveActor::ForceFail()
{
	SetObjectiveState(EState::Failed);
}

void AObjectiveActor::ActivateTracker(int32 TrackerIndex, bool bShouldActivate)
{
	AllTrackerComponents[TrackerIndex]->Activate(bShouldActivate);
}

UObjectiveTrackerComponent* AObjectiveActor::GetTracker(int32 TrackerIndex)
{
	return AllTrackerComponents[TrackerIndex]->IsValidLowLevel() ? AllTrackerComponents[TrackerIndex] : NULL;
}

void AObjectiveActor
::SetObjectiveState(EState NewState)
{
	ObjectiveState = NewState;

	if (GetObjectiveState() == EState::Succeed)
		SucceededEvent();
	if (GetObjectiveState() == EState::Failed)
		FailedEvent();

	CheckIfActivated();
}

