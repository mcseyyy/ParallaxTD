// Copyright VenoM Inc. 2014-2015

#pragma once
#include "Engine/Console.h"
#include "FXSystem.h"
#include "SubtitleManager.h"
#include "EngineModule.h"
#include "AudioDevice.h"
#include "ContentStreaming.h"				// Content streaming class definitions.
#include "StereoRendering.h"
#include "MyGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class VENOM_API UMyGameViewportClient : public UGameViewportClient
{
    GENERATED_UCLASS_BODY()

    virtual void Draw(FViewport* Viewport, FCanvas* SceneCanvas) override;

private:
    void UpdateSceneView(FSceneView* const View);
};
