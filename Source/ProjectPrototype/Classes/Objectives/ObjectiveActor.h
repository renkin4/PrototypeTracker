// Copyright AmmoboxStudios Sdn Bhd 2009-2018

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectiveActor.generated.h"

class UObjectiveTrackerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjSuccessDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjFailDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefreshUI);

UENUM(BlueprintType)
enum class EState : uint8
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

UCLASS()
class PROJECTPROTOTYPE_API AObjectiveActor : public AActor

{
	GENERATED_UCLASS_BODY()
protected:
	virtual void PreInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, Category = "Objective State")
	EState ObjectiveState;

	UPROPERTY(EditInstanceOnly, Category = "Objective Properties")
	FText ObjectiveName;

	UFUNCTION(BlueprintCallable, Category = "Objective Event")
	void SucceededEvent();

	UFUNCTION(BlueprintCallable, Category = "Objective Event")
	void FailedEvent();

private:
	TArray<UObjectiveTrackerComponent*> AllTrackerComponents;

	void SaveAllTrackerComponent();

	void CheckIfActivated();

public:
	UFUNCTION(BlueprintCallable, Category = "Objective State")
	void RunState();

	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnObjSuccessDelegate OnObjSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnObjFailDelegate OnObjFailedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnRefreshUI OnRefreshUI;

	//TODO Revamp this to push Data to UI instead
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Objective UI")
	void RefreshUI();

	UFUNCTION(BlueprintCallable, Category = "Objective State")
	void ForceSuccess();

	UFUNCTION(BlueprintCallable, Category = "Objective State")
	void ForceFail();

	UFUNCTION(BlueprintCallable, Category = "Objective Tracker")
	void ActivateTracker(int32 TrackerIndex, bool bShouldActivate);

	UFUNCTION(BlueprintPure, Category = "Objective Tracker")
	UObjectiveTrackerComponent*  GetTracker(int32 TrackerIndex);

	UFUNCTION(BlueprintPure, Category = "Objective State")
	const EState GetObjectiveState()
	{ return ObjectiveState; }

	UFUNCTION(BlueprintCallable, Category = "Objective State")
	void SetObjectiveState(EState NewState);

	UFUNCTION(BlueprintPure, Category = "Objective Property")
		const FText GetObjectiveName()
	{ return ObjectiveName; }

	UFUNCTION(BlueprintPure, Category = "Objective Tracker")
	TArray<UObjectiveTrackerComponent*> GetAllTracker()
	{ return AllTrackerComponents; }
};
