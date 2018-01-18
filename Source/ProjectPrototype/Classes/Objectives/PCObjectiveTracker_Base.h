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
			Track(TrackerProperties.TargettedActor, TrackerProperties.TargettedVariableName, TrackerProperties.TargettedActor2, TrackerProperties.TargettedVariableName2, false);
			break;
		case ETrackCondition::ActorVariable_Float:
			Track(TrackerProperties.TargettedActor, TrackerProperties.TargettedVariableName, TrackerProperties.TargettedActor2, TrackerProperties.TargettedVariableName2, 0.0f);
			break;
		case ETrackCondition::ActorVariable_Int:
			Track(TrackerProperties.TargettedActor, TrackerProperties.TargettedVariableName, TrackerProperties.TargettedActor2, TrackerProperties.TargettedVariableName2, 0);
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
			if (Val != Val2)
				bCompleted = true;
			break;
		case EComparison::Equal:
			if (Val == Val2)
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

	// Get Float Variable By Feeding Object Class and Variable Name
	static bool GetFloatByName(AActor* Target, FName VarName, float & outFloat)
	{
		if (Target)
		{
			float FoundFloat;
			UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);  
			if (FloatProp) //if we found variable
			{
				FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);  
				outFloat = FoundFloat;  // return float
				return true; // we can return
			}
		}//TODO add ERROR Message for Edison and TITO if Target Not Found
		return false;
	}

	// Get Int Variable By Feeding Object Class and Variable Name
	static bool GetIntByName(AActor * Target, FName VarName, int32 &outInt)
	{
		if (Target)
		{
			int32 FoundInt;
			UIntProperty* IntProp = FindField<UIntProperty>(Target->GetClass(), VarName); 
			if (IntProp)
			{
				FoundInt = IntProp->GetPropertyValue_InContainer(Target);
				outInt = FoundInt;
				return true;
			}
		}//TODO add ERROR Message for Edison and TITO if Target Not Found
		return false;
	}
	// Get bool Variable By Feeding Object Class and Variable Name
	static bool GetBoolByName(AActor * Target, FName VarName)
	{
		bool FoundBool = false;
		if (Target)
		{
			UBoolProperty* BoolProp = FindField<UBoolProperty>(Target->GetClass(), VarName);
			if (BoolProp)
			{
				FoundBool = BoolProp->GetPropertyValue_InContainer(Target);
			}
		}
		//TODO add ERROR Message for Edison and TITO if Target Not Found
		return FoundBool;
	}

	// Track Actor Loc to Actor Loc
	void Track(AActor* TargettedActor, AActor* TargettedActor2)
	{
		NumCal((TargettedActor->GetActorLocation() - TargettedActor2->GetActorLocation()).Size(), TrackerProperties.Distance);
	}

	// INT Track
	void Track(AActor* TargettedActor, FName TargettedName,AActor* TargettedActor2, FName TargettedName2,const int32 OutParaInt)
	{
		int32 Out, Out2;
		GetIntByName(TargettedActor, TargettedName, Out);
		GetIntByName(TargettedActor2, TargettedName2, Out2);
		NumCal(Out, Out2);
	}

	// Float Track
	void Track(AActor* TargettedActor, FName TargettedName, AActor* TargettedActor2, FName TargettedName2,const float OutParaFloat)
	{
		float Out, Out2;
		GetFloatByName(TargettedActor, TargettedName, Out);
		GetFloatByName(TargettedActor2, TargettedName2, Out2);
		NumCal(Out, Out2);
	}

	// Boolean track
	void Track(AActor* TargettedActor, FName TargettedName, AActor* TargettedActor2, FName TargettedName2,const bool OutParaBool)
	{
		//for boolean Section
		bool bOut = GetBoolByName(TargettedActor, TargettedName);
		bool bOut2 = GetBoolByName(TargettedActor2, TargettedName2);
		NumCal(bOut, bOut2);
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
