// Copyright Epic Games, Inc. All Rights Reserved.

#include "CameraPlaceTool.h"
#include "UI_CameraPlaceMenu.h"

#define LOCTEXT_NAMESPACE "FCameraPlaceToolModule"

void FCameraPlaceToolModule::StartupModule()
{
		RegisterCameraPlaceMenus();
}

void FCameraPlaceToolModule::ShutdownModule()
{
	UnregisterCameraPlaceMenus();
}

#undef LOCTEXT_NAMESPACE