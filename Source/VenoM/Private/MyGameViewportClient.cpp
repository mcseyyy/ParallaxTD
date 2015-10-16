// Copyright VenoM Inc. 2014-2015

#include "VenoM.h"
#include "MyGameViewportClient.h"


static FMatrix FrustumMatrix(float left, float right, float bottom, float top, float nearVal, float farVal)
{
    FMatrix Result;
    Result.SetIdentity();
    Result.M[0][0] = (2.0f * nearVal) / (right - left);
    Result.M[1][1] = (2.0f * nearVal) / (top - bottom);
    Result.M[2][0] = -(right + left) / (right - left);
    Result.M[2][1] = -(top + bottom) / (top - bottom);
    Result.M[2][2] = farVal / (farVal - nearVal);
    Result.M[2][3] = 1.0f;
    Result.M[3][2] = -(farVal * nearVal) / (farVal - nearVal);
    Result.M[3][3] = 0;

    return Result;
}

static FMatrix _AdjustProjectionMatrixForRHI(const FMatrix& InProjectionMatrix)
{
    const float GMinClipZ = 0.0f;
    const float GProjectionSignY = 1.0f;

    FScaleMatrix ClipSpaceFixScale(FVector(1.0f, GProjectionSignY, 1.0f - GMinClipZ));
    FTranslationMatrix ClipSpaceFixTranslate(FVector(0.0f, 0.0f, GMinClipZ));
    return InProjectionMatrix * ClipSpaceFixScale * ClipSpaceFixTranslate;
}

void UMyGameViewportClient::UpdateSceneView(FSceneView* const View)
{
    if (View == nullptr)
        return;

    View->ProjectionMatrixUnadjustedForRHI.M[0][0] *= 1.04;

    View->ViewMatrices.PreViewTranslation = -View->ViewMatrices.ViewOrigin;
    View->ViewMatrices.ProjMatrix = _AdjustProjectionMatrixForRHI(View->ProjectionMatrixUnadjustedForRHI);
    View->ViewProjectionMatrix = View->ViewMatrices.GetViewProjMatrix();
    View->InvViewProjectionMatrix = View->ViewMatrices.GetInvProjMatrix() * View->InvViewMatrix;
    FMatrix TranslatedViewMatrix = FTranslationMatrix(-View->ViewMatrices.PreViewTranslation) * View->ViewMatrices.ViewMatrix;
    View->ViewMatrices.TranslatedViewProjectionMatrix = TranslatedViewMatrix * View->ViewMatrices.ProjMatrix;
    View->ViewMatrices.InvTranslatedViewProjectionMatrix = View->ViewMatrices.TranslatedViewProjectionMatrix.Inverse();
    View->ShadowViewMatrices = View->ViewMatrices;

    //View->InvDeviceZToWorldZTransform = CreateInvDeviceZToWorldZTransform(View->ProjectionMatrixUnadjustedForRHI);

    GetViewFrustumBounds(View->ViewFrustum, View->ViewProjectionMatrix, false);
}


/** Whether to visualize the lightmap selected by the Debug Camera. */
extern ENGINE_API bool GShowDebugSelectedLightmap;
/** The currently selected component in the actor. */
extern ENGINE_API UPrimitiveComponent* GDebugSelectedComponent;
/** The lightmap used by the currently selected component, if it's a static mesh component. */
extern ENGINE_API class FLightMap2D* GDebugSelectedLightmap;

static TAutoConsoleVariable<int32> CVarSetBlackBordersEnabled(
    TEXT("r.BlackBorders"),
    0,
    TEXT("To draw black borders around the rendered image\n")
    TEXT("(prevents artifacts from post processing passes that read outside of the image e.g. PostProcessAA)\n")
    TEXT("in pixels, 0:off"),
    ECVF_Default);

/** Util to find named canvas in transient package, and create if not found */
static UCanvas* GetCanvasByName(FName CanvasName)
{
    // Cache to avoid FString/FName conversions/compares
    static TMap<FName, UCanvas*> CanvasMap;
    UCanvas** FoundCanvas = CanvasMap.Find(CanvasName);
    if (!FoundCanvas)
    {
        UCanvas* CanvasObject = FindObject<UCanvas>(GetTransientPackage(), *CanvasName.ToString());
        if (!CanvasObject)
        {
            CanvasObject = NewNamedObject<UCanvas>(GetTransientPackage(), CanvasName);
            CanvasObject->AddToRoot();
        }

        CanvasMap.Add(CanvasName, CanvasObject);
        return CanvasObject;
    }

    return *FoundCanvas;
}

/**
* Draw debug info on a game scene view.
*/
class FGameViewDrawer : public FViewElementDrawer
{
public:
    /**
    * Draws debug info using the given draw interface.
    */
    virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
    {
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
        // Draw a wireframe sphere around the selected lightmap, if requested.
        if (GShowDebugSelectedLightmap && GDebugSelectedComponent && GDebugSelectedLightmap)
        {
            float Radius = GDebugSelectedComponent->Bounds.SphereRadius;
            int32 Sides = FMath::Clamp<int32>(FMath::TruncToInt(Radius*Radius*4.0f*PI / (80.0f*80.0f)), 8, 200);
            DrawWireSphere(PDI, GDebugSelectedComponent->Bounds.Origin, FColor(255, 130, 0), GDebugSelectedComponent->Bounds.SphereRadius, Sides, SDPG_Foreground);
        }
#endif
    }
};


UMyGameViewportClient::UMyGameViewportClient(const class FObjectInitializer& PCIP)
    : Super(PCIP)
{
}

void UMyGameViewportClient::Draw(FViewport* InViewport, FCanvas* SceneCanvas)
{
    //Valid SceneCanvas is required.  Make this explicit.
    check(SceneCanvas);

    FCanvas* DebugCanvas = InViewport->GetDebugCanvas();

    // Create a temporary canvas if there isn't already one.
    static FName CanvasObjectName(TEXT("CanvasObject"));
    UCanvas* CanvasObject = GetCanvasByName(CanvasObjectName);
    CanvasObject->Canvas = SceneCanvas;

    // Create temp debug canvas object
    static FName DebugCanvasObjectName(TEXT("DebugCanvasObject"));
    UCanvas* DebugCanvasObject = GetCanvasByName(DebugCanvasObjectName);
    DebugCanvasObject->Canvas = DebugCanvas;
    DebugCanvasObject->Init(InViewport->GetSizeXY().X, InViewport->GetSizeXY().Y, NULL);

    bool bUIDisableWorldRendering = false;
    FGameViewDrawer GameViewDrawer;

    // create the view family for rendering the world scene to the viewport's render target
    FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
        InViewport,
        GetWorld()->Scene,
        EngineShowFlags)
        .SetRealtimeUpdate(true));

    ESplitScreenType::Type SplitScreenConfig = GetCurrentSplitscreenConfiguration();
    EngineShowFlagOverride(ESFIM_Game, (EViewModeIndex)ViewModeIndex, ViewFamily.EngineShowFlags, NAME_None, SplitScreenConfig != ESplitScreenType::None);

    TMap<ULocalPlayer*, FSceneView*> PlayerViewMap;

    FAudioDevice* AudioDevice = GEngine->GetAudioDevice();
    bool bReverbSettingsFound = false;
    FReverbSettings ReverbSettings;
    class AAudioVolume* AudioVolume = nullptr;

    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = *Iterator;
        if (PlayerController)
        {
            ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);
            if (LocalPlayer)
            {
                const bool bEnableStereo = GEngine->IsStereoscopic3D(InViewport);
                int32 NumViews = bEnableStereo ? 2 : 1;

                for (int i = 0; i < NumViews; ++i)
                {
                    // Calculate the player's view information.
                    FVector		ViewLocation;
                    FRotator	ViewRotation;

                    EStereoscopicPass PassType = !bEnableStereo ? eSSP_FULL : ((i == 0) ? eSSP_LEFT_EYE : eSSP_RIGHT_EYE);

                    FSceneView* View = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, InViewport, &GameViewDrawer, PassType);

                    UpdateSceneView(View);

                    if (View)
                    {
                        if (View->Family->EngineShowFlags.Wireframe)
                        {
                            // Wireframe color is emissive-only, and mesh-modifying materials do not use material substitution, hence...
                            View->DiffuseOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
                            View->SpecularOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
                        }
                        else if (View->Family->EngineShowFlags.OverrideDiffuseAndSpecular)
                        {
                            View->DiffuseOverrideParameter = FVector4(GEngine->LightingOnlyBrightness.R, GEngine->LightingOnlyBrightness.G, GEngine->LightingOnlyBrightness.B, 0.0f);
                            View->SpecularOverrideParameter = FVector4(.1f, .1f, .1f, 0.0f);
                        }
                        else if (View->Family->EngineShowFlags.ReflectionOverride)
                        {
                            View->DiffuseOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
                            View->SpecularOverrideParameter = FVector4(1, 1, 1, 0.0f);
                            View->NormalOverrideParameter = FVector4(0, 0, 1, 0.0f);
                            View->RoughnessOverrideParameter = FVector2D(0.0f, 0.0f);
                        }


                        if (!View->Family->EngineShowFlags.Diffuse)
                        {
                            View->DiffuseOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
                        }

                        if (!View->Family->EngineShowFlags.Specular)
                        {
                            View->SpecularOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
                        }

                        View->CameraConstrainedViewRect = View->UnscaledViewRect;

                        // If this is the primary drawing pass, update things that depend on the view location
                        if (i == 0)
                        {
                            // Save the location of the view.
                            LocalPlayer->LastViewLocation = ViewLocation;

                            PlayerViewMap.Add(LocalPlayer, View);

                            // Update the listener.
                            if (AudioDevice != NULL)
                            {
                                FVector Location;
                                FVector ProjFront;
                                FVector ProjRight;
                                PlayerController->GetAudioListenerPosition(/*out*/ Location, /*out*/ ProjFront, /*out*/ ProjRight);

                                FTransform ListenerTransform(FRotationMatrix::MakeFromXY(ProjFront, ProjRight));
                                ListenerTransform.SetTranslation(Location);
                                ListenerTransform.NormalizeRotation();

                                bReverbSettingsFound = true;

                                FReverbSettings PlayerReverbSettings;
                                FInteriorSettings PlayerInteriorSettings;
                                class AAudioVolume* PlayerAudioVolume = GetWorld()->GetAudioSettings(Location, &PlayerReverbSettings, &PlayerInteriorSettings);

                                if (AudioVolume == nullptr || (PlayerAudioVolume != nullptr && PlayerAudioVolume->Priority > AudioVolume->Priority))
                                {
                                    AudioVolume = PlayerAudioVolume;
                                    ReverbSettings = PlayerReverbSettings;
                                }

                                uint32 ViewportIndex = PlayerViewMap.Num() - 1;
                                AudioDevice->SetListener(ViewportIndex, ListenerTransform, (View->bCameraCut ? 0.f : GetWorld()->GetDeltaSeconds()), PlayerAudioVolume, PlayerInteriorSettings);
                            }

                        }

                        // Add view information for resource streaming.
                        IStreamingManager::Get().AddViewInformation(View->ViewMatrices.ViewOrigin, View->ViewRect.Width(), View->ViewRect.Width() * View->ViewMatrices.ProjMatrix.M[0][0]);
                        GetWorld()->ViewLocationsRenderedLastFrame.Add(View->ViewMatrices.ViewOrigin);
                    }
                }
            }
        }
    }

    if (bReverbSettingsFound)
    {
        AudioDevice->SetReverbSettings(AudioVolume, ReverbSettings);
    }

    // Update level streaming.
    GetWorld()->UpdateLevelStreaming();

    // Draw the player views.
    if (!bDisableWorldRendering && !bUIDisableWorldRendering && PlayerViewMap.Num() > 0)
    {
        GetRendererModule().BeginRenderingViewFamily(SceneCanvas, &ViewFamily);
    }

    // Clear areas of the rendertarget (backbuffer) that aren't drawn over by the views.
    {
        // Find largest rectangle bounded by all rendered views.
        uint32 MinX = InViewport->GetSizeXY().X, MinY = InViewport->GetSizeXY().Y, MaxX = 0, MaxY = 0;
        uint32 TotalArea = 0;
        for (int32 ViewIndex = 0; ViewIndex < ViewFamily.Views.Num(); ++ViewIndex)
        {
            const FSceneView* View = ViewFamily.Views[ViewIndex];

            FIntRect UpscaledViewRect = View->UnscaledViewRect;

            MinX = FMath::Min<uint32>(UpscaledViewRect.Min.X, MinX);
            MinY = FMath::Min<uint32>(UpscaledViewRect.Min.Y, MinY);
            MaxX = FMath::Max<uint32>(UpscaledViewRect.Max.X, MaxX);
            MaxY = FMath::Max<uint32>(UpscaledViewRect.Max.Y, MaxY);
            TotalArea += FMath::TruncToInt(UpscaledViewRect.Width()) * FMath::TruncToInt(UpscaledViewRect.Height());
        }

        // To draw black borders around the rendered image (prevents artifacts from post processing passes that read outside of the image e.g. PostProcessAA)
        {
            int32 BlackBorders = FMath::Clamp(CVarSetBlackBordersEnabled.GetValueOnGameThread(), 0, 10);

            if (ViewFamily.Views.Num() == 1 && BlackBorders)
            {
                MinX += BlackBorders;
                MinY += BlackBorders;
                MaxX -= BlackBorders;
                MaxY -= BlackBorders;
                TotalArea = (MaxX - MinX) * (MaxY - MinY);
            }
        }

        // If the views don't cover the entire bounding rectangle, clear the entire buffer.
        if (ViewFamily.Views.Num() == 0 || TotalArea != (MaxX - MinX)*(MaxY - MinY) || bDisableWorldRendering)
        {
            SceneCanvas->DrawTile(0, 0, InViewport->GetSizeXY().X, InViewport->GetSizeXY().Y, 0.0f, 0.0f, 1.0f, 1.f, FLinearColor::Black, NULL, false);
        }
        else
        {
            // clear left
            if (MinX > 0)
            {
                SceneCanvas->DrawTile(0, 0, MinX, InViewport->GetSizeXY().Y, 0.0f, 0.0f, 1.0f, 1.f, FLinearColor::Black, NULL, false);
            }
            // clear right
            if (MaxX < (uint32)InViewport->GetSizeXY().X)
            {
                SceneCanvas->DrawTile(MaxX, 0, InViewport->GetSizeXY().X, InViewport->GetSizeXY().Y, 0.0f, 0.0f, 1.0f, 1.f, FLinearColor::Black, NULL, false);
            }
            // clear top
            if (MinY > 0)
            {
                SceneCanvas->DrawTile(MinX, 0, MaxX, MinY, 0.0f, 0.0f, 1.0f, 1.f, FLinearColor::Black, NULL, false);
            }
            // clear bottom
            if (MaxY < (uint32)InViewport->GetSizeXY().Y)
            {
                SceneCanvas->DrawTile(MinX, MaxY, MaxX, InViewport->GetSizeXY().Y, 0.0f, 0.0f, 1.0f, 1.f, FLinearColor::Black, NULL, false);
            }
        }
    }

    // Remove temporary debug lines.
    if (GetWorld()->LineBatcher != NULL)
    {
        GetWorld()->LineBatcher->Flush();
    }

    if (GetWorld()->ForegroundLineBatcher != NULL)
    {
        GetWorld()->ForegroundLineBatcher->Flush();
    }

    // Draw FX debug information.
    if (GetWorld()->FXSystem)
    {
        GetWorld()->FXSystem->DrawDebug(SceneCanvas);
    }

    // Render the UI.
    {
        // render HUD
        bool bDisplayedSubtitles = false;
        for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
        {
            APlayerController* PlayerController = *Iterator;
            if (PlayerController)
            {
                ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);
                if (LocalPlayer)
                {
                    FSceneView* View = PlayerViewMap.FindRef(LocalPlayer);
                    if (View != NULL)
                    {
                        // rendering to directly to viewport target
                        FVector CanvasOrigin(FMath::TruncToFloat(View->UnscaledViewRect.Min.X), FMath::TruncToInt(View->UnscaledViewRect.Min.Y), 0.f);

                        CanvasObject->Init(View->UnscaledViewRect.Width(), View->UnscaledViewRect.Height(), View);

                        // Set the canvas transform for the player's view rectangle.
                        SceneCanvas->PushAbsoluteTransform(FTranslationMatrix(CanvasOrigin));
                        CanvasObject->ApplySafeZoneTransform();

                        // Render the player's HUD.
                        if (PlayerController->MyHUD)
                        {
                            DebugCanvasObject->SceneView = View;
                            PlayerController->MyHUD->SetCanvas(CanvasObject, DebugCanvasObject);
                            if (GEngine->IsStereoscopic3D(InViewport))
                            {
                                check(GEngine->StereoRenderingDevice.IsValid());
                                GEngine->StereoRenderingDevice->PushViewportCanvas(eSSP_LEFT_EYE, SceneCanvas, CanvasObject, Viewport);
                                PlayerController->MyHUD->PostRender();
                                SceneCanvas->PopTransform();

                                GEngine->StereoRenderingDevice->PushViewportCanvas(eSSP_RIGHT_EYE, SceneCanvas, CanvasObject, Viewport);
                                PlayerController->MyHUD->PostRender();
                                SceneCanvas->PopTransform();

                                // Reset the canvas for rendering to the full viewport.
                                CanvasObject->Reset();
                                CanvasObject->SizeX = View->UnscaledViewRect.Width();
                                CanvasObject->SizeY = View->UnscaledViewRect.Height();
                                CanvasObject->SetView(NULL);
                                CanvasObject->Update();
                            }
                            else
                            {
                                PlayerController->MyHUD->PostRender();
                            }

                            // Put these pointers back as if a blueprint breakpoint hits during HUD PostRender they can
                            // have been changed
                            CanvasObject->Canvas = SceneCanvas;
                            DebugCanvasObject->Canvas = DebugCanvas;

                            // A side effect of PostRender is that the playercontroller could be destroyed
                            if (!PlayerController->IsPendingKill())
                            {
                                PlayerController->MyHUD->SetCanvas(NULL, NULL);
                            }
                        }

                        if (DebugCanvas != NULL)
                        {
                            DebugCanvas->PushAbsoluteTransform(FTranslationMatrix(CanvasOrigin));
                            UDebugDrawService::Draw(ViewFamily.EngineShowFlags, InViewport, View, DebugCanvas);
                            DebugCanvas->PopTransform();
                        }

                        CanvasObject->PopSafeZoneTransform();
                        SceneCanvas->PopTransform();

                        // draw subtitles
                        if (!bDisplayedSubtitles)
                        {
                            FVector2D MinPos(0.f, 0.f);
                            FVector2D MaxPos(1.f, 1.f);
                            GetSubtitleRegion(MinPos, MaxPos);

                            uint32 SizeX = SceneCanvas->GetRenderTarget()->GetSizeXY().X;
                            uint32 SizeY = SceneCanvas->GetRenderTarget()->GetSizeXY().Y;
                            FIntRect SubtitleRegion(FMath::TruncToInt(SizeX * MinPos.X), FMath::TruncToInt(SizeY * MinPos.Y), FMath::TruncToInt(SizeX * MaxPos.X), FMath::TruncToInt(SizeY * MaxPos.Y));
                            // We need a world to do this
                            FSubtitleManager::GetSubtitleManager()->DisplaySubtitles(SceneCanvas, SubtitleRegion, GetWorld()->GetAudioTimeSeconds());
                        }
                    }
                }
            }
        }

        //ensure canvas has been flushed before rendering UI
        SceneCanvas->Flush_GameThread();
        if (DebugCanvas != NULL)
        {
            DebugCanvas->Flush_GameThread();
        }
        // Allow the viewport to render additional stuff
        PostRender(DebugCanvasObject);

        // Render the console.
        if (ViewportConsole)
        {
            if (GEngine->IsStereoscopic3D(InViewport))
            {
                GEngine->StereoRenderingDevice->PushViewportCanvas(eSSP_LEFT_EYE, DebugCanvas, DebugCanvasObject, Viewport);
                ViewportConsole->PostRender_Console(DebugCanvasObject);
                DebugCanvas->PopTransform();

                GEngine->StereoRenderingDevice->PushViewportCanvas(eSSP_RIGHT_EYE, DebugCanvas, DebugCanvasObject, Viewport);
                ViewportConsole->PostRender_Console(DebugCanvasObject);
                DebugCanvas->PopTransform();

                // Reset the canvas for rendering to the full viewport.
                DebugCanvasObject->Reset();
                DebugCanvasObject->SizeX = Viewport->GetSizeXY().X;
                DebugCanvasObject->SizeY = Viewport->GetSizeXY().Y;
                DebugCanvasObject->SetView(NULL);
                DebugCanvasObject->Update();
            }
            else
            {
                ViewportConsole->PostRender_Console(DebugCanvasObject);
            }
        }
    }


    // Grab the player camera location and orientation so we can pass that along to the stats drawing code.
    FVector PlayerCameraLocation = FVector::ZeroVector;
    FRotator PlayerCameraRotation = FRotator::ZeroRotator;
    {
        for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
        {
            (*Iterator)->GetPlayerViewPoint(PlayerCameraLocation, PlayerCameraRotation);
        }
    }

    if (GEngine->IsStereoscopic3D(InViewport))
    {
        GEngine->StereoRenderingDevice->PushViewportCanvas(eSSP_LEFT_EYE, DebugCanvas, DebugCanvasObject, InViewport);
        DrawStatsHUD(GetWorld(), InViewport, DebugCanvas, DebugCanvasObject, DebugProperties, PlayerCameraLocation, PlayerCameraRotation);
        DebugCanvas->PopTransform();

        GEngine->StereoRenderingDevice->PushViewportCanvas(eSSP_RIGHT_EYE, DebugCanvas, DebugCanvasObject, InViewport);
        DrawStatsHUD(GetWorld(), InViewport, DebugCanvas, DebugCanvasObject, DebugProperties, PlayerCameraLocation, PlayerCameraRotation);
        DebugCanvas->PopTransform();

        // Reset the canvas for rendering to the full viewport.
        DebugCanvasObject->Reset();
        DebugCanvasObject->SizeX = Viewport->GetSizeXY().X;
        DebugCanvasObject->SizeY = Viewport->GetSizeXY().Y;
        DebugCanvasObject->SetView(NULL);
        DebugCanvasObject->Update();
    }
    else
    {
        DrawStatsHUD(GetWorld(), InViewport, DebugCanvas, DebugCanvasObject, DebugProperties, PlayerCameraLocation, PlayerCameraRotation);
    }
}
