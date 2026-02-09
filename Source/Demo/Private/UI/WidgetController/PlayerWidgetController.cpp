// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/PlayerWidgetController.h"

void UPlayerWidgetController::SetWidgetControllerParmas(const FWidgetControllerParmas& WCParmas)
{
	PlayerController = WCParmas.PlayerCcontroller;
	PlayerState = WCParmas.PlayerState;
	AbilitySystemComponent = WCParmas.AbilitySystemComponent;
	AttributeSet = WCParmas.AttributeSet;
}
