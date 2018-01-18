// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StateMachine/PCSM_State.h"
#include "Engine/DataAsset.h"
#include "PCObjective_Base.generated.h"


UENUM()
enum class EObjectivesCompletion : uint8
{
	EQC_NotStarted,
	EQC_Started,
	EQC_Succeeded,
	EQC_Failed
};

/**
*
*/
UCLASS()
class PROJECTPROTOTYPE_API UObjectives : public UDataAsset
{
	GENERATED_BODY()

public:

	// The name of the UObjectives.
	UPROPERTY(EditAnywhere)
	FText ObjectivesName;

	// If this machine accepts our ObjectivesActivities log, the Objectives is successful.
	UPROPERTY(EditAnywhere)
	UPCSM_State* ObjectivesStateMachine;

	// If true, the InputList is a blacklist. Otherwise, it's a whitelist.
	UPROPERTY(EditAnywhere)
	uint32 bInputBlackList : 1;

	// The blacklist/whitelist (depending on bBlackList) used to filter InputAtoms this UObjectives recognizes.
	UPROPERTY(EditAnywhere)
	TArray<UPCSM_InputAtom*> InputList;
};


USTRUCT()
struct FObjectivesInProgress
{
	GENERATED_BODY()

	// UObjectives data asset.
	UPROPERTY(EditAnywhere)
	const UObjectives* Objectives;

	// Current progress in the UObjectives.
	UPROPERTY(EditAnywhere)
	EObjectivesCompletion ObjectivesProgress;

protected:
	// All input for this Objectives. Filtered by the UObjectives's blacklist/whitelist.
	UPROPERTY(EditAnywhere)
	TArray<UPCSM_InputAtom*> ObjectivesActivities;

public:

	// Returns true if the UObjectives was completed (success or failure) by the new activity.
	bool UpdateObjectives(const UObject* ObjectRef, UPCSM_InputAtom* ObjectivesActivity)
	{
		// Only log activity to valid, in-progress Objectivess. Check the blacklist/whitelist before logging activity.
		if (Objectives && (ObjectivesProgress == EObjectivesCompletion::EQC_Started)
			&& (Objectives->bInputBlackList != Objectives->InputList.Contains(ObjectivesActivity)))
		{
			FStateMachineResult ObjectivesResult;
			ObjectivesActivities.Add(ObjectivesActivity);
			ObjectivesResult = Objectives->ObjectivesStateMachine->RunState(ObjectRef, ObjectivesActivities);
			switch (ObjectivesResult.CompletionType)
			{
			case EStateMachineCompletionType::Accepted:
				ObjectivesProgress = EObjectivesCompletion::EQC_Succeeded;
				return true;
			case EStateMachineCompletionType::Rejected:
				ObjectivesProgress = EObjectivesCompletion::EQC_Failed;
				return true;
				// case NotAccepted: // Still in progress. No update needed.
			}
		}
		return false;
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTPROTOTYPE_API AObjectivesStatus : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	AObjectivesStatus();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Add to our UObjectives activity log! This also automatically checks to see if any
	unfinished UObjectivess are now complete. */
	UFUNCTION(BlueprintCallable, Category = "UObjectivess")
	void UpdateObjectivess(UPCSM_InputAtom* ObjectivesActivity);

protected:
	// The master list of all UObjectives-related things we've done.
	UPROPERTY(EditAnywhere)
	TArray<UPCSM_InputAtom*> ObjectivesActivities;

	// The list of UObjectivess in our current game or area.
	UPROPERTY(EditAnywhere)
	TArray<FObjectivesInProgress> ObjectivesList;
};
