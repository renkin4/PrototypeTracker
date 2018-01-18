// Fill out your copyright notice in the Description page of Project Settings.

#include "StateMachine/PCSM_State.h"


UPCSM_State::UPCSM_State()
{
	bLoopByDefault = true;
}

UPCSM_State* UPCSM_Branch::TryBranch(const UObject* RefObject,
	const TArray<UPCSM_InputAtom*>& DataSource, int32 DataIndex, int32 &OutDataIndex)
{
	OutDataIndex = DataIndex + 1;
	if (DataSource.IsValidIndex(DataIndex) && AcceptableInputs.Contains(DataSource[DataIndex]))
	{
		return bReverseInputTest ? nullptr : DestinationState;
	}
	return bReverseInputTest ? DestinationState : nullptr;
}

FStateMachineResult UPCSM_State::RunState(const UObject* RefObject,
	const TArray<UPCSM_InputAtom*>& DataSource, int32 DataIndex /*= 0*/, int32 RemainingSteps /*= -1*/)
{
	bool bMustEndNow = (bTerminateImmediately || !DataSource.IsValidIndex(DataIndex));

	// If we're still running, see where our branches lead.
	if (RemainingSteps && !bMustEndNow)
	{
		UPCSM_State* DestinationState = nullptr;
		int32 DestinationDataIndex = DataIndex;
		for (int32 i = 0; i < InstancedBranches.Num(); ++i)
		{
			// This could be a check. There shouldn't be null branches in the list.
			if (InstancedBranches[i])
			{
				DestinationState = InstancedBranches[i]->TryBranch(RefObject,
					DataSource, DataIndex, DestinationDataIndex);
				if (DestinationState)
				{
					return DestinationState->RunState(RefObject, DataSource, DestinationDataIndex, RemainingSteps - 1);
				}
			}
		}
		// We didn't find any acceptable branch, so we must end on this state unless we're told to loop.
		if (bLoopByDefault)
		{
			return LoopState(RefObject, DataSource, DataIndex, RemainingSteps);
		}
		bMustEndNow = true;
	}

	FStateMachineResult SMR;
	SMR.FinalState = this;
	SMR.DataIndex = DataIndex;
	SMR.CompletionType = bMustEndNow ? CompletionType : EStateMachineCompletionType::OutOfSteps;
	return SMR;
}

FStateMachineResult UPCSM_State::LoopState(const UObject* RefObject,
	const TArray<UPCSM_InputAtom*>& DataSource, int32 DataIndex, int32 RemainingSteps)
{
	// By default, increase DataIndex by 1 and decrease RemainingSteps by 1.
	return RunState(RefObject, DataSource, DataIndex + 1, RemainingSteps - 1);
}
