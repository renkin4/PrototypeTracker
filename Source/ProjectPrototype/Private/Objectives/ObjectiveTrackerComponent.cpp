// Copyright AmmoboxStudios Sdn Bhd 2009-2018

#include "Objectives/ObjectiveTrackerComponent.h"
#include "Objectives/ObjectiveActor.h"
#include "UObject/UnrealType.h"

// Sets default values for this component's properties
UObjectiveTrackerComponent::UObjectiveTrackerComponent(const class FObjectInitializer & PCIP)
	:Super(PCIP)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bCompleted = false;
	Comparison = EComparison::MoreThen;
	SetComponentTickEnabled(false);
	// ...
}

void UObjectiveTrackerComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UObjectiveTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RunTracker();
}

void UObjectiveTrackerComponent::RunTracker()
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
		Track(TrackerProperties.WorldTimeLimit);
		break;
	case ETrackCondition::ActorKilled:
		Track(TrackerProperties.TargettedActor);
		break;
	default:
		break;
	}
}

void UObjectiveTrackerComponent::OnCompleted()
{
	AObjectiveActor* ObjectiveActor = Cast<AObjectiveActor>(GetOuter());

	if (ObjectiveActor)
	{
		if (bSuccess)
			ObjectiveActor->ForceSuccess();
		else
			ObjectiveActor->ForceFail();

		OnTrackerFinishesDelegate.Broadcast();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s Has Completed it's Tracker"), *this->GetName()));

		SetComponentTickEnabled(false);
	}
	else
	{
		/*FString ErrorMsg = FString::FString("Don't Add Tracker On ");
		ErrorMsg.Append(this->GetName());
		print(ErrorMsg);*/
	}
}


const float UObjectiveTrackerComponent::GetFloatByName(const AActor * Target, const FName VarName)
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
	}
	else
	{
		/*FString ErrorMsg = FString::FString("ERROR:: NO Target Actor On ");
		ErrorMsg.Append(this->GetName());
		print(ErrorMsg);*/
	}
	return FoundFloat;
}

const int32 UObjectiveTrackerComponent::GetIntByName(const AActor * Target, const FName VarName)
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
	}
	else
	{
		/*FString ErrorMsg = FString::FString("ERROR:: NO Target Actor On ");
		ErrorMsg.Append(this->GetName());
		print(ErrorMsg);*/
	}
	return FoundInt;
}

const bool UObjectiveTrackerComponent::GetBoolByName(const AActor * Target, const FName VarName)
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
	else
	{
		/*FString ErrorMsg = FString::FString("ERROR:: NO Target Actor On ");
		ErrorMsg.Append(this->GetName());
		print(ErrorMsg);*/
	}
	return FoundBool;
}

void UObjectiveTrackerComponent::Track(const AActor * TargettedActor, const AActor * TargettedActor2)
{
	NumCal((TargettedActor->GetActorLocation() - TargettedActor2->GetActorLocation()).Size(), TrackerProperties.Distance);
}

void UObjectiveTrackerComponent::Track(const AActor * TargettedActor, const FName TargettedName, const AActor * TargettedActor2, const FName TargettedName2, const int32 DummyInt)
{
	int32 Out = GetIntByName(TargettedActor, TargettedName);
	int32 Out2 = GetIntByName(TargettedActor2, TargettedName2);
	NumCal(Out, Out2);
}

void UObjectiveTrackerComponent::Track(const AActor * TargettedActor, const FName TargettedName, const AActor * TargettedActor2, const FName TargettedName2, const float DummyFloat)
{
	float Out = GetFloatByName(TargettedActor, TargettedName);
	float Out2 = GetFloatByName(TargettedActor2, TargettedName2);
	NumCal(Out, Out2);
}

void UObjectiveTrackerComponent::Track(const AActor * TargettedActor, const FName TargettedName, const AActor * TargettedActor2, const FName TargettedName2, const bool DummyBool)
{
	//for boolean Section
	bool bOut = GetBoolByName(TargettedActor, TargettedName);
	bool bOut2 = GetBoolByName(TargettedActor2, TargettedName2);
	NumCal(bOut, bOut2);
}

void UObjectiveTrackerComponent::Track(const float InsertedTime)
{
	NumCal(GetWorld()->TimeSeconds, InsertedTime);
}

void UObjectiveTrackerComponent::Track(AActor * SelectedActor)
{
	/*GPUI = Cast<IPCGameplayUnitInterface>(SelectedActor);
	GPUC = GPUI->Execute_IGetGameplayComponent(SelectedActor);

	if (GPUC)
	{
		if (!GPUC->IsAlive())
			SetCompleted(true);

		return;
	}
	else
	{
		FString ErrorMsg = FString::FString("ERROR:: NO Gameplay Unit Component Found On ");
		ErrorMsg.Append(this->GetName());
		print(ErrorMsg);
	}

	if (!SelectedActor)
		SetCompleted(true);
*/
}

void UObjectiveTrackerComponent::SetCompleted(bool bShouldComplete)
{
	bCompleted = bShouldComplete;
	if (GetCompleted())
		OnCompleted();
}