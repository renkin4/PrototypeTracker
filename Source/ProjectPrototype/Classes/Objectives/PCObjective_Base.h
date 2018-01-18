// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PCObjective_Base.generated.h"

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
	//test Are
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Test")
	UObject * Target;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Test")
	FName VarName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Test")
	float Storage;
};
