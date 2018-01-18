// Fill out your copyright notice in the Description page of Project Settings.

#include "Objectives/PCObjective_Base.h"


// Sets default values
APCObjective_Base::APCObjective_Base(const class FObjectInitializer & PCIP)
	:Super(PCIP)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APCObjective_Base::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APCObjective_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Target) //make sure Target was set in blueprints. 
	{
		float FoundFloat;
		UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);  // try to find float property in Target named VarName
		if (FloatProp) //if we found variable
		{
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);  // get the value from FloatProp
			Storage = FoundFloat;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), Storage));
		}
	}
}

