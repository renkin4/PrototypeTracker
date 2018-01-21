// Fill out your copyright notice in the Description page of Project Settings.

#include "Objectives/PCObjectiveTracker_Base.h"
#include "Objectives/PCObjective_Base.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UPCObjectiveTracker_Base::UPCObjectiveTracker_Base(const class FObjectInitializer & PCIP)
	:Super(PCIP)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bCompleted = false;
	Comparison = EComparison::MoreThen;
	// ...
}

void UPCObjectiveTracker_Base::BeginPlay() 
{
	Super::BeginPlay();
}

// Called every frame
void UPCObjectiveTracker_Base::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RunTracker();
}

void UPCObjectiveTracker_Base::RunTracker()
{
	APCObjective_Base * ObjActor = Cast<APCObjective_Base>(GetOuter());
	if (GetCompleted() || !ObjActor)
		return;

	// Need find a better way to Deactivate this component to reduce Including that file
	if(ObjActor->GetObjectiveState() != EState::Activated)
		return;

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
		Track(TrackerProperties.WorldTimeLimit);
		break;
	default:
		break;
	}
}

void UPCObjectiveTracker_Base::OnCompleted()
{
	OnTrackerFinishesDelegate.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s Has Completed it's Tracker"), *this->GetName()));

	// Not Working
	Activate(false);
}

const float UPCObjectiveTracker_Base::GetFloatByName(const AActor * Target, const FName VarName)
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

const int32 UPCObjectiveTracker_Base::GetIntByName(const AActor * Target, const FName VarName)
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

const bool UPCObjectiveTracker_Base::GetBoolByName(const AActor * Target, const FName VarName)
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

void UPCObjectiveTracker_Base::Track(const AActor * TargettedActor, const AActor * TargettedActor2)
{
	NumCal((TargettedActor->GetActorLocation() - TargettedActor2->GetActorLocation()).Size(), TrackerProperties.Distance);
}

void UPCObjectiveTracker_Base::Track(const AActor * TargettedActor, const FName TargettedName, const AActor * TargettedActor2, const FName TargettedName2, const int32 DummyInt)
{
	int32 Out = GetIntByName(TargettedActor, TargettedName);
	int32 Out2 = GetIntByName(TargettedActor2, TargettedName2);
	NumCal(Out, Out2);
}

void UPCObjectiveTracker_Base::Track(const AActor * TargettedActor, const FName TargettedName, const AActor * TargettedActor2, const FName TargettedName2, const float DummyFloat)
{
	float Out = GetFloatByName(TargettedActor, TargettedName);
	float Out2 = GetFloatByName(TargettedActor2, TargettedName2);
	NumCal(Out, Out2);
}

void UPCObjectiveTracker_Base::Track(const AActor * TargettedActor, const FName TargettedName, const AActor * TargettedActor2, const FName TargettedName2, const bool DummyBool)
{
	//for boolean Section
	bool bOut = GetBoolByName(TargettedActor, TargettedName);
	bool bOut2 = GetBoolByName(TargettedActor2, TargettedName2);
	NumCal(bOut, bOut2);
}

void UPCObjectiveTracker_Base::Track(const float InsertedTime)
{
	NumCal(GetWorld()->TimeSeconds, InsertedTime);
}

void UPCObjectiveTracker_Base::SetCompleted(bool bShouldComplete)
{
	bCompleted = bShouldComplete;
	if (GetCompleted())
		OnCompleted();
}



