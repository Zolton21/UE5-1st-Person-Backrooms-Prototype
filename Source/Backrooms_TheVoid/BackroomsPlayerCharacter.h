// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "BackroomsPlayerCharacter.generated.h"


class UInputAction;
class UInputMappingContext;

UCLASS()
class BACKROOMS_THEVOID_API ABackroomsPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABackroomsPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* PlayerCharacterMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveForwardAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveRightAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float BaseWalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ForwardSprintSpeedMultiplier = 1.6f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ForwardSideSprintSpeedMultiplier = 1.4f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ForwardSpeedMultiplier = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float ForwardSideSpeedMultiplier = 0.95f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SideSpeedMultiplier = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SideBackwardsSpeedMultiplier = 0.65f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float BackwardSpeedMultiplier = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	APlayerController* CharacterPlayerController;
	bool bIsSprinting = false;
	bool bIsMovingForward = false;
	float ForwardInputValue = 0;
	float RightInputValue = 0;

	void Look(const FInputActionValue& Value);
	void MoveForward(const FInputActionValue& Value);
	void ResetForwardInputValue();
	void MoveRight(const FInputActionValue& Value);
	void ResetRightInputValue();
	void UpdateMovementSpeed();
	void StartSprinting(const FInputActionValue& Value);
	void StopSprinting(const FInputActionValue& Value);
	bool IsDead() const;

	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
