// Fill out your copyright notice in the Description page of Project Settings.


#include "BackroomsPlayerController.h"
#include "Blueprint/UserWidget.h"


void ABackroomsPlayerController::BeginPlay() {
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World) {
		FString CurrentMapName = World->GetMapName();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *CurrentMapName);
		if (CurrentMapName.EndsWith("Level0")) {
			if (WorldGenerationScreenClass != nullptr) {
				HUD = CreateWidget(this, WorldGenerationScreenClass);
				if (HUD) {
					HUD->AddToViewport();

					bShowMouseCursor = true;

					FInputModeUIOnly InputMode;
					InputMode.SetWidgetToFocus(HUD->TakeWidget());
					InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

					SetInputMode(InputMode);
				}
			}
		}
	}
}

void ABackroomsPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner) {
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	UUserWidget* Screen;
	if (HUD != nullptr) {
		HUD->RemoveFromViewport();
	}

	if (bIsWinner == true) {
		Screen = CreateWidget(this, WinScreenClass);
	}
	else {
		Screen = CreateWidget(this, LoseScreenClass);
	}

	if (Screen != nullptr) {
		Screen->AddToViewport();
	}
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, 5.0f);
}