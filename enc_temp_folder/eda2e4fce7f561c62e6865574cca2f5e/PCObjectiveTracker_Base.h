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
	DistanceBetweenActor,
	ActorVariable_Float,

	//Not Working Yet
	ActorVariable_Vector,

	//Not Working Yet
	ActorVariable_Boolean,
	ActorVariable_Int,

	//Not Working Yet
	ActorArrayLength_Int
};

USTRUCT(BlueprintType)
struct PROJECTPROTOTYPE_API FTrackerProperties
{
	GENERATED_USTRUCT_BODY()
	// The Distance between 2 Values that's inserted
	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
	float Distance;

	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
	AActor* TargettedActor;

	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
	FName TargettedVariableName;

	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
	AActor* TargettedActor2;

	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
	FName TargettedVariableName2;
	

	FTrackerProperties()
	{
		Distance = 0.f;
		TargettedActor = NULL;
		TargettedActor2 = NULL;
		TargettedVariableName = TEXT("");
		TargettedVariableName2 = TEXT("");
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
		case ETrackCondition::ActorVariable_Boolean:

			break;
		case ETrackCondition::ActorVariable_Float:
			Track(TrackerProperties.TargettedActor, TrackerProperties.TargettedVariableName, TrackerProperties.TargettedActor2, TrackerProperties.TargettedVariableName2);
			break;
		case ETrackCondition::ActorVariable_Int:
			Track(TrackerProperties.TargettedActor, TrackerProperties.TargettedVariableName, TrackerProperties.TargettedActor2, TrackerProperties.TargettedVariableName2);
			break;
		default:
			break;
		}
	}

private:
	// Compare between 2 Numbers. Used Template due to Difference of Datatype
	template <class TNumbers>
	TNumbers NumCal(TNumbers Val, TNumbers Val2)
	{
		switch (Comparison)
		{
		case EComparison::NotEqual:
			if (Val == Val2)
				bCompleted = true;
			break;
		case EComparison::Equal:
			if (Val != Val2)
				bCompleted = true;
			break;
		case EComparison::MoreThen:
			if (Val > Val2)
				bCompleted = true;
			break;
		case EComparison::LessThen:
			if (Val < Val2)
				bCompleted = true;
			break;
		case EComparison::NotExist:
			break;
		default:
			break;
		}
		return bCompleted;
	}

	// Get Variable By Feeding Object Class and Variable Name
	static bool GetFloatByName(AActor* Target, FName VarName, float & outFloat)
	{
		if (Target) //make sure Target was set in blueprints. 
		{
			float FoundFloat;
			UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);  // try to find float property in Target named VarName
			if (FloatProp) //if we found variable
			{
				FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);  // get the value from FloatProp
				outFloat = FoundFloat;  // return float
				return true; // we can return
			}
		}
		return false;
	}

	static bool GetIntByName(AActor * Target, FName VarName, int32 &outInt)
	{
		if (Target)
		{
			int32 FoundInt;
			UIntProperty* IntProp = FindField<UIntProperty>(Target->GetClass(), VarName);  //this time I'm using UIntProperty as I'm searching for int
			if (IntProp)
			{
				FoundInt = IntProp->GetPropertyValue_InContainer(Target);
				outInt = FoundInt;
				return true;
			}
		}
		return false;
	}

	// Track Actor Loc to Actor Loc
	void Track(AActor* TargettedActor, AActor* TargettedActor2)
	{
		NumCal((TargettedActor->GetActorLocation() - TargettedActor2->GetActorLocation()).Size(), TrackerProperties.Distance);
	}

	// Track Actor Loc to Actor Loc
	void Track(AActor* TargettedActor, FName TargettedName,AActor* TargettedActor2, FName TargettedName2)
	{
		if (TrackCondition == ETrackCondition::ActorArrayLength_Int) 
		{
			int32 Out;
			int32 Out2;
			GetIntByName(TargettedActor, TargettedName, Out);
			GetIntByName(TargettedActor2, TargettedName2, Out2);
			NumCal(Out, Out2);
		}
		if (TrackCondition == ETrackCondition::ActorVariable_Float) 
		{
			float Out;
			float Out2;
			GetFloatByName(TargettedActor, TargettedName, Out);
			GetFloatByName(TargettedActor2, TargettedName2, Out2);
			NumCal(Out, Out2);
		}
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
