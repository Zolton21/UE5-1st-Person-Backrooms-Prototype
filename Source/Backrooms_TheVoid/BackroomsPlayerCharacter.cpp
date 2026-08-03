// Fill out your copyright notice in the Description page of Project Settings.


#include "BackroomsPlayerCharacter.h"
#include "BackroomsPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputAction.h"
#include "BackroomsGameMode.h"


// Sets default values
ABackroomsPlayerCharacter::ABackroomsPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABackroomsPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	CharacterPlayerController = Cast<ABackroomsPlayerController>(GetController());
	if (CharacterPlayerController) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(CharacterPlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(PlayerCharacterMappingContext, 0);
		}
	}

	Health = MaxHealth;
}

// Called every frame
void ABackroomsPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABackroomsPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABackroomsPlayerCharacter::Look);

		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ABackroomsPlayerCharacter::MoveForward);
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &ABackroomsPlayerCharacter::ResetForwardInputValue);

		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ABackroomsPlayerCharacter::MoveRight);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Completed, this, &ABackroomsPlayerCharacter::ResetRightInputValue);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABackroomsPlayerCharacter::StartSprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABackroomsPlayerCharacter::StopSprinting);
	}
}

void ABackroomsPlayerCharacter::Look(const FInputActionValue& Value) {
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (GetController()) {
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(-LookAxisValue.Y);
	}
}

void ABackroomsPlayerCharacter::MoveForward(const FInputActionValue& Value) {
	ForwardInputValue = Value.Get<float>();

	UpdateMovementSpeed();
	AddMovementInput(GetActorForwardVector(), ForwardInputValue);
}

void ABackroomsPlayerCharacter::ResetForwardInputValue() {
	ForwardInputValue = 0;
	UpdateMovementSpeed();
}

void ABackroomsPlayerCharacter::MoveRight(const FInputActionValue& Value) {
	RightInputValue = Value.Get<float>();
	
	UpdateMovementSpeed();
	AddMovementInput(GetActorRightVector(), RightInputValue);
}

void ABackroomsPlayerCharacter::ResetRightInputValue() {
	RightInputValue = 0;
	UpdateMovementSpeed();
}

void ABackroomsPlayerCharacter::UpdateMovementSpeed() {
	if (RightInputValue != 0) {
		if (ForwardInputValue > 0) {
			//forward-side running
			if (bIsSprinting == true) {
				GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * ForwardSideSprintSpeedMultiplier;
				return;
			}
			//forward-side walking
			else {
				GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * ForwardSideSpeedMultiplier;
				return;
			}
		}
		//backward-side
		if (ForwardInputValue < 0) {
			GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * SideBackwardsSpeedMultiplier;
			return;
		}
		//side
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * SideSpeedMultiplier;
		return;
	}
	if (ForwardInputValue > 0) {
		//forward running
		if (bIsSprinting == true) {
			GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * ForwardSprintSpeedMultiplier;
			return;
		}
		//forward walking
		else {
			GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * ForwardSpeedMultiplier;
			return;
		}
	}
	//backward
	if (ForwardInputValue < 0) {
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * BackwardSpeedMultiplier;
		return;
	}
}

void ABackroomsPlayerCharacter::StartSprinting(const FInputActionValue& Value) {
	bIsSprinting = true;
}

void ABackroomsPlayerCharacter::StopSprinting(const FInputActionValue& Value) {
	bIsSprinting = false;
}

float ABackroomsPlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health = -DamageToApply;

	if (IsDead()) {
		ABackroomsGameMode* GameMode = GetWorld()->GetAuthGameMode<ABackroomsGameMode>();
		if (GameMode != nullptr) {
			GameMode->PlayerKilled(this);
		}
	}
	return DamageToApply;
}

bool ABackroomsPlayerCharacter::IsDead() const {
	return Health <= 0;
}

