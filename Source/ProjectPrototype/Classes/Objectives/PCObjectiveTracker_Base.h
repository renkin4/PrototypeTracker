// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "PCStateType.h"
#include "PCObjectiveTracker_Base.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrackSuccessDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrackFailDelegate);

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
	ActorVariable_Boolean,
	ActorVariable_Int,

	//Not Working Yet
	ActorArrayLength_Int,
	GameTime
};

// Properties of Trackers that will be grab by pointing on actors in the world
USTRUCT(BlueprintType)
struct PROJECTPROTOTYPE_API FTrackerProperties
{
	GENERATED_USTRUCT_BODY()
	// The Distance between 2 Values that's inserted
	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
	float Distance;

	// Choose the Time Limit you wish to compare to
	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
	float GameTime;

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
//Branch for objectives
USTRUCT(BlueprintType)
struct PROJECTPROTOTYPE_API FTrackerBranch
{
	GENERATED_USTRUCT_BODY()

	AActor* MyOwner;

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
		case ETrackCondition::GameTime:
			Track(TrackerProperties.GameTime);
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
	const float GetFloatByName(const AActor* Target, const FName VarName)
	{
		float FoundFloat = FLT_MAX;
		if (Target)
		{
			UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);  
			if (FloatProp) //if we found variable
			{
				FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);  
				return FoundFloat; // we can return
			}
		}//TODO add ERROR Message for Edison and TITO if Target Not Found
		return FoundFloat;
	}

	// Get Int Variable By Feeding Object Class and Variable Name
	const int32 GetIntByName(const AActor * Target, const FName VarName)
	{
		int32 FoundInt = MAX_int32;
		if (Target)
		{
			UIntProperty* IntProp = FindField<UIntProperty>(Target->GetClass(), VarName); 
			if (IntProp)
			{
				FoundInt = IntProp->GetPropertyValue_InContainer(Target);
				return FoundInt;
			}
		}//TODO add ERROR Message for Edison and TITO if Target Not Found
		return FoundInt;
	}

	// Get bool Variable By Feeding Object Class and Variable Name
	const bool GetBoolByName(const AActor * Target, const FName VarName)
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
	void Track(const AActor* TargettedActor, const AActor* TargettedActor2)
	{
		NumCal((TargettedActor->GetActorLocation() - TargettedActor2->GetActorLocation()).Size(), TrackerProperties.Distance);
	}

	// INT Track
	void Track(const AActor* TargettedActor, const FName TargettedName, const AActor* TargettedActor2, const FName TargettedName2,const int32 DummyInt)
	{
		int32 Out = GetIntByName(TargettedActor, TargettedName);
		int32 Out2 = GetIntByName(TargettedActor2, TargettedName2);
		NumCal(Out, Out2);
	}

	// Float Track
	void Track(const AActor* TargettedActor, const FName TargettedName, const AActor* TargettedActor2, const FName TargettedName2,const float DummyFloat)
	{
		float Out = GetFloatByName(TargettedActor, TargettedName);
		float Out2 = GetFloatByName(TargettedActor2, TargettedName2);
		NumCal(Out, Out2);
	}

	// Boolean track
	void Track(const AActor* TargettedActor, const FName TargettedName, const AActor* TargettedActor2, const FName TargettedName2,const bool DummyBool)
	{
		//for boolean Section
		bool bOut = GetBoolByName(TargettedActor, TargettedName);
		bool bOut2 = GetBoolByName(TargettedActor2, TargettedName2);
		NumCal(bOut, bOut2);
	}

	void Track(float InsertedTime) 
	{
		//Because i can't seem to get the world Time I save it through DeltaTime
		// TODO refactor these into a seperate Class
		NumCal(MyOwner->GetWorld()->TimeSeconds, InsertedTime);
	}
};

UCLASS( ClassGroup=(Tracker), meta=(BlueprintSpawnableComponent) , Blueprintable)
class PROJECTPROTOTYPE_API UPCObjectiveTracker_Base : public UActorComponent
{
	GENERATED_UCLASS_BODY()
protected:
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Current State of the Tracker
	EState CurrentTrackState;

	//Branches for Multiple task Objectives
	UPROPERTY(EditInstanceOnly, Category = "Tracker Branches")
	FTrackerBranch TrackerBranches;

	// Check if this Branch is Optional Tracker or Critical
	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
	uint8 bCriticalTracker : 1;

	//Events on Objective Succeeded
	void SucceededEvent();

	//Event on Objective Failed
	void FailedEvent();

public:
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnTrackSuccessDelegate OnTrackerSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnTrackFailDelegate OnTrackerFailedDelegate;

	//Set Tracker State and send signal to check if Succeeded
	UFUNCTION(BlueprintCallable, Category = "Tracker States")
	void SetTrackerState(EState SetState);

	//Get Current State For Tracker
	UFUNCTION(BlueprintPure, Category = "Tracker States")
	EState GetTrackerState() { return CurrentTrackState; }

	//Activate Objective
	UFUNCTION(BlueprintCallable, Category = "Tracker States")
	void RunState() { CurrentTrackState = EState::Activated; }

	FTrackerBranch GetTrackerBranch() { return TrackerBranches; }

	bool GetIsCriticalTracker() { return bCriticalTracker; }
};
