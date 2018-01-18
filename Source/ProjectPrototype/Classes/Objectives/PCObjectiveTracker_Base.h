// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PCObjectiveTracker_Base.generated.h"

UENUM(BlueprintType)
enum class ETrackerState : uint8
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

UENUM(BlueprintType)
enum class EComparison : uint8
{
	/*
	* This Enum will compare between pointed Actor in the world and the variables given
	*/
	NotEqual,
	Equal,
	MoreThen,
	LessThen,
	NotExist
};

UENUM(BlueprintType)
enum class ETrackCondition : uint8
{
	/*
	* This Will Determine what to track
	*/
	DistanceBetweenActor
};

USTRUCT(BlueprintType)
struct PROJECTPROTOTYPE_API FTrackerProperties
{
	GENERATED_USTRUCT_BODY()
	// The Distance between 2 Values that's inserted
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Tracker Properties")
	float Distance;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Tracker Properties")
	AActor* TargettedActor;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Tracker Properties")
	AActor* TargettedActor2;

	FTrackerProperties()
	{
		Distance = 0.f;
		TargettedActor = NULL;
		TargettedActor2 = NULL;
	}
};
//TODO shift this branch into another Class
//TODO Refactor due to redundancy
//Branch for objectives
USTRUCT(BlueprintType)
struct PROJECTPROTOTYPE_API FTrackerBranch
{
	GENERATED_USTRUCT_BODY()

	// Track Condition are Types of Variable you wish to used to compare with
	UPROPERTY(EditInstanceOnly, Category = "Tracker")
	ETrackCondition TrackCondition;

	// Properties that you wish to compare with
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Tracker Properties")
	FTrackerProperties TrackerProperties;

	// Different Parameters to Compare Different Variables
	UPROPERTY(EditInstanceOnly, Category = "Tracker")
	EComparison Comparison;

	// If set false, When Branch Completed will be forced to break loop and Set State to Fail
	UPROPERTY(EditInstanceOnly, Category = "Tracker")
	uint8 bSuccessBranch : 1;

	uint8 bCompleted : 1;

	// Constructor
	FTrackerBranch() 
	{
		bSuccessBranch = false;
		TrackerProperties = FTrackerProperties();
		TrackCondition = ETrackCondition::DistanceBetweenActor;
	}

	// Start Tracking
	void RunBranch() 
	{
		switch (TrackCondition)
		{
		case ETrackCondition::DistanceBetweenActor:
			Track(TrackerProperties.TargettedActor, TrackerProperties.TargettedActor2);
			break;
		default:
			break;
		}
	}

private:
	void VectorDistanceCal(FVector Loc, FVector Loc2) 
	{
		switch (Comparison)
		{
		case EComparison::NotEqual:
			if ((Loc - Loc2).Size() == TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::Equal:
			if ((Loc - Loc2).Size() != TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::MoreThen:
			if ((Loc - Loc2).Size() > TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::LessThen:
			if ((Loc - Loc2).Size() < TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::NotExist:
			break;
		default:
			break;
		}
	}

	// Track Actor Loc to Actor Loc
	void Track(AActor* TargettedActor, AActor* ActorToCompare) 
	{
		VectorDistanceCal(TargettedActor->GetActorLocation(), ActorToCompare->GetActorLocation());
	}
};

UCLASS( ClassGroup=(Tracker), meta=(BlueprintSpawnableComponent) , Blueprintable)
class PROJECTPROTOTYPE_API UPCObjectiveTracker_Base : public UActorComponent
{
	GENERATED_UCLASS_BODY()
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Current State of the Tracker
	ETrackerState CurrentTrackState;

	//Branches for Multiple task Objectives
	UPROPERTY(EditInstanceOnly, Category = "Tracker Branches")
	FTrackerBranch TrackerBranches;

	//Events on Objective Succeeded
	void SucceededEvent();

	//Event on Objective Failed
	void FailedEvent();

public:
	//Set Tracker State and send signal to check if Succeeded
	UFUNCTION(BlueprintCallable, Category = "Tracker States")
	void SetTrackerState(ETrackerState SetState);

	//Get Current State For Tracker
	UFUNCTION(BlueprintPure, Category = "Tracker States")
	ETrackerState GetTrackerState() { return CurrentTrackState; }

	//Activate Objective
	UFUNCTION(BlueprintCallable, Category = "Tracker States")
	void RunState() { CurrentTrackState = ETrackerState::Activated; }

};
