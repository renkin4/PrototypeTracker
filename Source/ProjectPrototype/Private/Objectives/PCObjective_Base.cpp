// Fill out your copyright notice in the Description page of Project Settings.

#include "Objectives/PCObjective_Base.h"



// Sets default values for this component's properties
AObjectivesStatus::AObjectivesStatus()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void AObjectivesStatus::BeginPlay()
{
	Super::BeginPlay();
	// ...

}


// Called every frame
void AObjectivesStatus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ...
}

void AObjectivesStatus::UpdateObjectivess(UPCSM_InputAtom * ObjectivesActivity)
{
	// Update the master list of everything we've ever done.
	ObjectivesActivities.Add(ObjectivesActivity);

	// Update individual quests (if they care about this activity) and see if they are complete.
	for (int32 i = ObjectivesList.Num() - 1; i >= 0; --i)
	{
		if (ObjectivesList[i].UpdateObjectives(this, ObjectivesActivity))
		{
			UE_LOG(LogTemp, Warning, TEXT("Quest \"%s\" completed: %s"),
				*ObjectivesList[i].Objectives->ObjectivesName.ToString(),
				(ObjectivesList[i].ObjectivesProgress == EObjectivesCompletion::EQC_Succeeded) ? TEXT("Success") : TEXT("Failure"));
		}
	}
}

