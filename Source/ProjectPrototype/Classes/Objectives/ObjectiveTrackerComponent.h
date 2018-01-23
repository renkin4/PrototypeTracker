#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectiveTrackerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrackerFinishesDelegate);

UENUM(BlueprintType)
enum class EComparison : uint8
{
	/*
	* This Enum will compare between pointed Actor in the world and the variables given
	*/
	NotEqual,
	Equal,
	MoreThen,
	LessThen
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
	GameTime,
	ActorKilled
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
		float WorldTimeLimit;

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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class PROJECTPROTOTYPE_API UObjectiveTrackerComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// To briefly explain what player have to know to complete this tracker
	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
		FText TrackerDescription;

	// For UI to show player's the Requirement Needed to complete this tracker
	FText TrackerRequirement;

	// Track Condition are Types of Variable you wish to used to compare with
	UPROPERTY(EditInstanceOnly, Category = "Tracker")
	ETrackCondition TrackCondition;

	// Different Parameters to Compare Different Variables
	UPROPERTY(EditInstanceOnly, Category = "Tracker")
	EComparison Comparison;

	// Properties that you wish to compare with
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Tracker Properties")
	FTrackerProperties TrackerProperties;

	UPROPERTY(EditInstanceOnly, Category = "Tracker Properties")
	uint8 bSuccess : 1;

private:
	void OnCompleted();

	// Compare between 2 Numbers. Used Template due to Difference of Datatype
	template <class TNumbers>
	FORCEINLINE TNumbers NumCal(TNumbers Val, TNumbers Val2)
	{
		switch (Comparison)
		{
		case EComparison::NotEqual:
			if (Val != Val2)
				SetCompleted(true);
			break;
		case EComparison::Equal:
			if (Val == Val2)
				SetCompleted(true);
			break;
		case EComparison::MoreThen:
			if (Val > Val2)
				SetCompleted(true);
			break;
		case EComparison::LessThen:
			if (Val < Val2)
				SetCompleted(true);
			break;
		default:
			break;
		}
		return GetCompleted();
	}

	// Start Tracking
	void RunTracker();

	// Boolean to Check if tracker Completed it's Task
	uint8 bCompleted : 1;

	// Get Float Variable By Feeding Object Class and Variable Name
	const float GetFloatByName(const AActor* Target, const FName VarName);

	// Get Int Variable By Feeding Object Class and Variable Name
	const int32 GetIntByName(const AActor * Target, const FName VarName);

	// Get bool Variable By Feeding Object Class and Variable Name
	const bool GetBoolByName(const AActor * Target, const FName VarName);

	// Track Actor Loc to Actor Loc
	void Track(const AActor* TargettedActor, const AActor* TargettedActor2);

	// INT Track
	void Track(const AActor* TargettedActor, const FName TargettedName, const AActor* TargettedActor2, const FName TargettedName2, const int32 DummyInt);

	// Float Track
	void Track(const AActor* TargettedActor, const FName TargettedName, const AActor* TargettedActor2, const FName TargettedName2, const float DummyFloat);

	// Boolean track
	void Track(const AActor* TargettedActor, const FName TargettedName, const AActor* TargettedActor2, const FName TargettedName2, const bool DummyBool);

	// Time Tracker
	void Track(const float InsertedTime);

	// Track an actor existence in the world
	void Track(AActor * SelectedActor);

	//TODO track Internal Time

public:
	// Use this to Bind to a Function that you wish to happen on Tracker Finishes
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnTrackerFinishesDelegate OnTrackerFinishesDelegate;

	// Set Completed State
	UFUNCTION(BlueprintCallable, Category = "Tracker State")
	void SetCompleted(bool bShouldComplete);

	// Get the State of the Tracker
	UFUNCTION(BlueprintPure, Category = "Tracker State")
	const bool GetCompleted()
	{ return bCompleted; }

	UFUNCTION(BlueprintPure, Category = "Tracker Info")
		FText GetTrackerDescription()
	{ return TrackerDescription; }

	UFUNCTION(BlueprintPure, Category = "Tracker Info")
		FText GetTrackerRequirement()
	{ return TrackerRequirement; }

};
