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
	FloatToFloat,
	VectorToVector,
	ActorToActor,
	VectorToActor
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

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Tracker Properties")
	FVector TargettedVector;

	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
	FVector TargettedVector2;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Tracker Properties")
	float FloatVal;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Tracker Properties")
	float FloatVal2;

	FTrackerProperties()
	{
		Distance = 0.f;
		TargettedActor = NULL;
		TargettedActor2 = NULL;
		TargettedVector = FVector();
		TargettedVector2 = FVector();
		FloatVal = 0.f;
		FloatVal2 = 0.f;
	}
};
//TODO shift this branch into another Class
//TODO Refactor due to redundancy
//Branch for objectives
USTRUCT(BlueprintType)
struct PROJECTPROTOTYPE_API FTrackerBranch
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Tracker")
	ETrackCondition TrackCondition;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Tracker Properties")
	FTrackerProperties TrackerProperties;

	UPROPERTY(EditInstanceOnly, Category = "Tracker")
	EComparison Comparison;

	UPROPERTY(EditInstanceOnly, Category = "Tracker")
	uint8 bSuccessBranch : 1;

	uint8 bCompleted : 1;
	FTrackerBranch() 
	{
		bSuccessBranch = false;
		bCompleted = false;
		TrackerProperties = FTrackerProperties();
		TrackCondition = ETrackCondition::ActorToActor;
	}
	void RunBranch() 
	{
		switch (TrackCondition)
		{
		case ETrackCondition::ActorToActor:
			Track(TrackerProperties.TargettedActor, TrackerProperties.TargettedActor2);
			break;
		case ETrackCondition::FloatToFloat:
			Track(TrackerProperties.FloatVal, TrackerProperties.FloatVal2);
			break;
		case ETrackCondition::VectorToActor:
			Track(TrackerProperties.TargettedVector, TrackerProperties.TargettedActor);
			break;
		case ETrackCondition::VectorToVector:
			Track(TrackerProperties.TargettedVector, TrackerProperties.TargettedVector2);
			break;
		default:
			break;
		}
	}
private:
	// Track Float To Float
	void Track(float TargettedFloat, float FloatToCompare) 
	{
		switch (Comparison)
		{
		case EComparison::NotEqual:
			if (TargettedFloat != FloatToCompare)
				bCompleted = true;
			break;
		case EComparison::Equal:
			if (TargettedFloat == FloatToCompare)
				bCompleted = true;
			break;
		case EComparison::MoreThen:
			if (TargettedFloat > FloatToCompare)
				bCompleted = true;
			break;
		case EComparison::LessThen:
			if (TargettedFloat < FloatToCompare)
				bCompleted = true;
			break;
		case EComparison::NotExist:
			break;
		default:
			break;
		}
	}
	// Track Vector To Vector
	void Track(FVector TargettedVector, FVector VectorToCompare) 
	{
		switch (Comparison)
		{
		case EComparison::NotEqual:
			if ((TargettedVector - VectorToCompare).Size() == TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::Equal:
			if ((TargettedVector - VectorToCompare).Size() != TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::MoreThen:
			if ((TargettedVector - VectorToCompare).Size() > TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::LessThen:
			if ((TargettedVector - VectorToCompare).Size() < TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::NotExist:
			break;
		default:
			break;
		}
	}
	// Track Vector to Actor Loc
	void Track(FVector TargettedVector, AActor* ActorToCompare) 
	{
		switch (Comparison)
		{
		case EComparison::NotEqual:
			if ((TargettedVector - ActorToCompare->GetActorLocation()).Size() != TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::Equal:
			if ((TargettedVector - ActorToCompare->GetActorLocation()).Size() == TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::MoreThen:
			if ((TargettedVector - ActorToCompare->GetActorLocation()).Size() > TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::LessThen:
			if ((TargettedVector - ActorToCompare->GetActorLocation()).Size() < TrackerProperties.Distance)
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
		switch (Comparison)
		{
		case EComparison::NotEqual:
			if ((TargettedActor->GetActorLocation() - ActorToCompare->GetActorLocation()).Size() != TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::Equal:
			if ((TargettedActor->GetActorLocation() - ActorToCompare->GetActorLocation()).Size() == TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::MoreThen:
			if ((TargettedActor->GetActorLocation() - ActorToCompare->GetActorLocation()).Size() > TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::LessThen:
			if ((TargettedActor->GetActorLocation() - ActorToCompare->GetActorLocation()).Size() < TrackerProperties.Distance)
				bCompleted = true;
			break;
		case EComparison::NotExist:
			break;
		default:
			break;
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

	UPROPERTY(EditInstanceOnly, Category = "Tracker States")
	ETrackerState CurrentTrackState;

	UPROPERTY(EditInstanceOnly, Category = "Tracker Branches")
	TArray<FTrackerBranch> TrackerBranches;

	void SucceededEvent();
	void FailedEvent();
public:
	//Set Tracker State and send signal to check if Succeeded
	UFUNCTION(BlueprintCallable, Category = "Tracker States")
	void SetTrackerState(ETrackerState SetState);

	UFUNCTION(BlueprintPure, Category = "Tracker States")
	ETrackerState GetTrackerState() { return CurrentTrackState; }

	UFUNCTION(BlueprintCallable, Category = "Tracker States")
	void RunState() { CurrentTrackState = ETrackerState::Activated; }

};
