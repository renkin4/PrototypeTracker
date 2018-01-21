// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PCStateType.h"
#include "PCObjective_Base.generated.h"

class UPCObjectiveTracker_Base;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjSuccessDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjFailDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefreshUI);


UCLASS()
class PROJECTPROTOTYPE_API APCObjective_Base : public AActor
{
	GENERATED_UCLASS_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Objective State")
	EState ObjectiveState;

	UPROPERTY(EditInstanceOnly, Category = "Objective Properties")
	FText ObjectiveName;

	UFUNCTION(BlueprintCallable, Category = "Objective Event")
	void SucceededEvent();

	UFUNCTION(BlueprintCallable, Category = "Objective Event")
	void FailedEvent();

	UFUNCTION(BlueprintCallable, Category = "Objective Event")
	void OnAllCriticalMissionSucceeded();
private :
	TArray<UPCObjectiveTracker_Base*> AllTrackerComponents;

	TArray<UPCObjectiveTracker_Base*> SucceededTrackerHolder;

	TArray<UPCObjectiveTracker_Base*> FailedTrackerHolder;

	int32 AmountOfCriticalTrackerSuccess;

	void SaveAllTrackerComponent();

public:
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnObjSuccessDelegate OnObjSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnObjFailDelegate OnObjFailedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnRefreshUI OnRefreshUI;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Objective UI")
	void RefreshUI();

	UFUNCTION(BlueprintCallable, Category = "Objective State")
	void ForceSuccess();

	UFUNCTION(BlueprintCallable, Category = "Objective State")
	void ForceFail();

	UFUNCTION(BlueprintCallable, Category = "Objective State")
	void RunCheckList();

	UFUNCTION(BlueprintCallable, Category = "Objective Tracker")
	void ActivateTracker(int32 TrackerIndex);

	UFUNCTION(BlueprintPure, Category = "Objective Tracker")
	UPCObjectiveTracker_Base*  GetTracker(int32 TrackerIndex);

	UFUNCTION(BlueprintPure, Category = "Objective State")
	EState GetObjectiveState() { return ObjectiveState; }

	UFUNCTION(BlueprintCallable, Category = "Objective State")
	void SetObjectiveState(EState NewState);

	UFUNCTION(BlueprintPure, Category = "Objective Property")
	FText GetObjectiveName() { return ObjectiveName; }
};
