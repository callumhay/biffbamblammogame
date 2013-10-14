/**
 * InGameRenderPipeline.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "InGameRenderPipeline.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameFontAssetsManager.h"
#include "LivesLeftHUD.h"
#include "PointsHUD.h"
#include "BallBoostHUD.h"
#include "BallReleaseHUD.h"
#include "RemoteControlRocketHUD.h"
#include "CgFxCelOutlines.h"

#include "../BlammoEngine/Camera.h"
#include "../GameModel/GameModel.h"
#include "../GameControl/GameControllerManager.h"

const unsigned int InGameRenderPipeline::HUD_X_INDENT = 10;	
const unsigned int InGameRenderPipeline::HUD_Y_INDENT = 10;

InGameRenderPipeline::InGameRenderPipeline(GameDisplay* display) : display(display) {
	assert(display != NULL);
}

InGameRenderPipeline::~InGameRenderPipeline() {
}
	
void InGameRenderPipeline::RenderFrameWithoutHUD(double dT) {

	this->SetupRenderFrame(dT);
	this->ApplyInGameCamera(dT);

    Matrix4x4 gameTransform  = this->display->GetModel()->GetTransformInfo()->GetGameXYZTransform();
    Vector2D negHalfLevelDim = -0.5f * this->display->GetModel()->GetLevelUnitDimensions();

    this->display->GetSound()->SetGameFGTransform(gameTransform);

	FBObj* backgroundFBO = this->RenderBackgroundToFBO(negHalfLevelDim, dT);
	this->RenderForegroundToFBO(negHalfLevelDim, gameTransform, backgroundFBO, dT);
	this->RenderFinalGather(negHalfLevelDim, gameTransform, dT);
}

void InGameRenderPipeline::RenderFrame(double dT) {
	this->RenderFrameWithoutHUD(dT);
	this->RenderHUD(dT);
}

void InGameRenderPipeline::SetupRenderFrame(double dT) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Tick the assets (update them for amount of elapsed time dT).
	this->display->GetAssets()->Tick(dT);
}

// Apply the in-game camera to the current world - this places the player camera in the correct
// position for rendering the world from their POV.
void InGameRenderPipeline::ApplyInGameCamera(double dT) {
	// Camera Stuff 
	Camera& camera = this->display->GetCamera();
	camera.SetPerspectiveWithFOV(this->display->GetModel()->GetTransformInfo()->GetCameraFOVAngle());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// TODO: since we now reset the camera and reapply its transform each frame, we need to tell the MODEL about debug transforms!!
#ifdef _DEBUG
	if (!GameDisplay::IsCameraDetached()) {
		camera.SetTransform(this->display->GetModel()->GetTransformInfo()->GetCameraTransform());
	}
#else
	camera.SetTransform(this->display->GetModel()->GetTransformInfo()->GetCameraTransform());
#endif	
	camera.ApplyCameraTransform();

    // Don't apply the camera shake if we're in bullet time mode, also clear any existing camera shakes
    if (this->display->GetModel()->GetBallBoostModel() != NULL &&
        this->display->GetModel()->GetBallBoostModel()->IsInBulletTime()) {
        camera.ClearCameraShake();
        GameControllerManager::GetInstance()->ClearControllerVibration();
    }
    else {
        camera.ApplyCameraShakeTransform(dT);
    }
}

// Render just the background (includes the skybox, background geometry and effects), the rendering
// will be done to the background FBO in the FBO assets and a pointer to it will be returned
// NOTE: The caller does NOT assume ownership of the returned memory
FBObj* InGameRenderPipeline::RenderBackgroundToFBO(const Vector2D& negHalfLevelDim, double dT) {
	UNUSED_PARAMETER(dT);

    GameAssets* assets = this->display->GetAssets();
    GameFBOAssets* fboAssets = assets->GetFBOAssets();

    const Camera& camera = this->display->GetCamera();

	FBObj* backgroundFBO = fboAssets->GetBackgroundFBO();
	assert(backgroundFBO != NULL);

    FBObj* colourAndDepthFBO = fboAssets->GetColourAndDepthTexFBO();
    assert(colourAndDepthFBO != NULL);

    // Render the background geometry into the colour and depth FBO
    colourAndDepthFBO->BindFBObj();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Draw the background of the current scene
	glPushMatrix();
	glTranslatef(0.0f, negHalfLevelDim[1], 0.0f);

	assets->DrawSkybox(camera);
	assets->DrawBackgroundModel(camera);
 
    // Render the outlines using the special cel outline effect on what we just rendered...
    {
        const GameWorldAssets* currWorldAssets = assets->GetCurrentWorldAssets();
        static const float OUTLINE_AMBIENT_BRIGHTNESS = 1.0f;

        CgFxCelOutlines& celOutlineEffect = fboAssets->GetCelOutlineEffect();
        celOutlineEffect.SetMinDistance(currWorldAssets->GetOutlineMinDistance());
        celOutlineEffect.SetMaxDistance(currWorldAssets->GetOutlineMaxDistance());
        celOutlineEffect.SetContrastExponent(currWorldAssets->GetOutlineContrast());
        celOutlineEffect.SetOffsetMultiplier(currWorldAssets->GetOutlineOffset());
        celOutlineEffect.SetAlphaMultiplier(assets->GetCurrentWorldAssets()->GetAlpha());
        celOutlineEffect.SetAmbientBrightness(OUTLINE_AMBIENT_BRIGHTNESS);
        celOutlineEffect.Draw(colourAndDepthFBO, backgroundFBO);
    }

	// Draw background effects into the background FBO -- we do this as a separate pass because
    // if we include it in the previous pass, the outlines will show through all the effects (which is not so pretty)
    backgroundFBO->BindFBObj();
    
    assets->DrawBackgroundEffects(camera);
    glPopMatrix();

	backgroundFBO->UnbindFBObj();

	debug_opengl_state();
	return backgroundFBO;
}

FBObj* InGameRenderPipeline::RenderForegroundToFBO(const Vector2D& negHalfLevelDim, const Matrix4x4& gameTransform, FBObj* backgroundFBO, double dT) {
	assert(backgroundFBO != NULL);

	const Camera& camera = this->display->GetCamera();
    GameModel* gameModel = this->display->GetModel();
	const GameLevel* currLevel = gameModel->GetCurrentLevel();

    GameAssets* assets = this->display->GetAssets();
    GameFBOAssets* fboAssets = assets->GetFBOAssets();
    
    FBObj* colourAndDepthFBO = fboAssets->GetColourAndDepthTexFBO();
	FBObj* fullSceneFBO = fboAssets->GetFullSceneFBO();
    
    assert(colourAndDepthFBO != NULL);
	assert(fullSceneFBO      != NULL);

    colourAndDepthFBO->BindFBObj();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//backgroundFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);
    backgroundFBO->GetFBOTexture()->RenderTextureToFullscreenQuadNoDepth();
    //glClear(GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glMultMatrixf(gameTransform.begin());

    // Bosses
    assets->DrawBoss(dT, currLevel, camera);

    // Safety net (if active)
    assets->DrawSafetyNetIfActive(dT, camera, *gameModel);

	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

    // Special embedded labels...
    this->DrawSpecialEmbeddedLabels(dT);

    // Paddle
	assets->DrawPaddle(dT, *gameModel->GetPlayerPaddle(), camera);
	glPopMatrix();

	// Level pieces / blocks and their associated emitter effects
	assets->DrawLevelPieces(dT, currLevel, camera);

	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Balls
	assets->DrawGameBalls(dT, *gameModel, camera, negHalfLevelDim);

    // Mesh projectiles (e.g., rockets, mines)
    assets->DrawMeshProjectiles(dT, *gameModel, camera);

	glPopMatrix();

    glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Draw the dropping items if not in the last pass
	if (!fboAssets->DrawItemsInLastPass()) {
		std::list<GameItem*>& gameItems = gameModel->GetLiveItems();
		for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); ++iter) {
			assets->DrawItem(dT, camera, (**iter));
		}				
	}

	// Render the beam effects
	assets->DrawBeams(*gameModel, camera);

    // Add outlines to the scene...
    {
        static const float OUTLINE_MIN_DIST = 0.0001f;
        static const float OUTLINE_MAX_DIST = 5.0f;
        static const float OUTLINE_CONTRAST = 1.0f;
        static const float OUTLINE_OFFSET = 0.75f;

        CgFxCelOutlines& celOutlineEffect = fboAssets->GetCelOutlineEffect();
        celOutlineEffect.SetMinDistance(OUTLINE_MIN_DIST);
        celOutlineEffect.SetMaxDistance(OUTLINE_MAX_DIST);
        celOutlineEffect.SetContrastExponent(OUTLINE_CONTRAST);
        celOutlineEffect.SetOffsetMultiplier(OUTLINE_OFFSET);
        celOutlineEffect.SetAlphaMultiplier(assets->GetCurrentLevelMesh()->GetLevelAlpha());
        celOutlineEffect.SetAmbientBrightness(1.0f);
        celOutlineEffect.Draw(colourAndDepthFBO, fullSceneFBO);
    }

	// Draw Post-full-scene effects (N.B., when you bind a new FBO, the old one is automatically unbound)
    fullSceneFBO->BindFBObj();

	// Render any post-processing effects for various items/objects in the game
	assets->DrawProjectiles(dT, camera);
    assets->DrawPaddlePostEffects(dT, *gameModel, camera, colourAndDepthFBO);
    assets->DrawLevelPiecesPostEffects(dT, camera);
	assets->DrawStatusEffects(dT, camera, colourAndDepthFBO);
    assets->DrawMiscEffects(*gameModel);
    glPopMatrix();

    // Tesla lightning arcs
    assets->DrawTeslaLightning(dT, camera);
    glPopMatrix();

	FBObj::UnbindFBObj();

	// Do a Gaussian blur for a softer feeling
	assets->GetFBOAssets()->RenderFullSceneBlur(camera.GetWindowWidth(), camera.GetWindowHeight(), dT);

	debug_opengl_state();

    return fullSceneFBO;
}

void InGameRenderPipeline::RenderFinalGather(const Vector2D& negHalfLevelDim, const Matrix4x4& gameTransform, double dT) {

    GameAssets* assets       = this->display->GetAssets();
	GameFBOAssets* fboAssets = assets->GetFBOAssets();
	GameModel* gameModel     = this->display->GetModel();
	const Camera& camera     = this->display->GetCamera();

	// Render full-screen effects
    // N.B., Bloom is applied here
	FBObj* initialFBO = fboAssets->RenderInitialFullscreenEffects(camera.GetWindowWidth(), camera.GetWindowHeight(), dT);
    FBObj* colourAndDepthFBO = fboAssets->GetColourAndDepthTexFBO();
    FBObj* finalFBO = fboAssets->GetFinalFullScreenFBO();
    
    assert(initialFBO != NULL);
    assert(colourAndDepthFBO != NULL);
    assert(finalFBO != NULL);
   
	// Final non-full-screen draw pass - draw the falling items and particles
	colourAndDepthFBO->BindFBObj();
    initialFBO->GetFBOTexture()->RenderTextureToFullscreenQuadNoDepth();

	// Render all effects that do not go through all the post-processing filters...

    glPushMatrix();
    glMultMatrixf(gameTransform.begin());
    glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0);

    // Item drop blocks (draw them without bloom because otherwise it's hard to see
    // what item they're dropping)
    assets->DrawNoBloomLevelPieces(dT, camera);
    
    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw the dropping items if in the last pass
    if (fboAssets->DrawItemsInLastPass()) {
        std::list<GameItem*>& gameItems = gameModel->GetLiveItems();
        for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); ++iter) {
            assets->DrawItem(dT, camera, (**iter));
        }
    }

	// Typical Particle effects...
	GameESPAssets* espAssets = assets->GetESPAssets();
	espAssets->DrawBeamEffects(dT, camera, Vector3D(negHalfLevelDim,0));
	espAssets->DrawParticleEffects(dT, camera);

	// Absolute post effects call for various object effects
	assets->DrawGameBallsPostEffects(dT, *gameModel, camera);

    // Add outlines to the scene...
    {
        static const float OUTLINE_MIN_DIST = 0.001f;
        static const float OUTLINE_MAX_DIST = 5.0f;
        static const float OUTLINE_CONTRAST = 1.0f;
        static const float OUTLINE_OFFSET = 0.75f;

        CgFxCelOutlines& celOutlineEffect = fboAssets->GetCelOutlineEffect();
        celOutlineEffect.SetMinDistance(OUTLINE_MIN_DIST);
        celOutlineEffect.SetMaxDistance(OUTLINE_MAX_DIST);
        celOutlineEffect.SetContrastExponent(OUTLINE_CONTRAST);
        celOutlineEffect.SetOffsetMultiplier(OUTLINE_OFFSET);
        celOutlineEffect.SetAlphaMultiplier(assets->GetCurrentLevelMesh()->GetLevelAlpha());
        celOutlineEffect.SetAmbientBrightness(1.0f);
        celOutlineEffect.Draw(colourAndDepthFBO, finalFBO);
    }

	// Render the final fullscreen effects
	fboAssets->RenderFinalFullscreenEffects(camera.GetWindowWidth(), camera.GetWindowHeight(), dT, *gameModel);

	glPopMatrix();
	debug_opengl_state();
}

void InGameRenderPipeline::RenderHUD(double dT) {
    
    GameAssets* gameAssets = this->display->GetAssets();
    GameModel* gameModel   = this->display->GetModel();
	const Camera& camera   = this->display->GetCamera();

	const int DISPLAY_WIDTH  = camera.GetWindowWidth();
	const int DISPLAY_HEIGHT = camera.GetWindowHeight();

    // Draw the points HUD
    // NOTE: No points or multipliers for boss levels
    const GameLevel* currLevel = gameModel->GetCurrentLevel();
    if (!currLevel->GetHasBoss()) {
	    // Draw the points in the top-right corner of the display
        PointsHUD* pointsHUD = gameAssets->GetPointsHUD();
        pointsHUD->Draw(camera, DISPLAY_WIDTH, DISPLAY_HEIGHT, dT);
    }

	// Draw the number of lives left in the top-left corner of the display
	gameAssets->GetLifeHUD()->Draw(dT, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	// Draw the ball boost HUD display in the top-left corner, under the number of balls left
    BallBoostHUD* boostHUD = gameAssets->GetBoostHUD();
    boostHUD->Draw(camera, gameModel->GetBallBoostModel(), DISPLAY_HEIGHT, dT);

    // Draw the ball release timer HUD display
    BallReleaseHUD* ballReleaseHUD = gameAssets->GetBallReleaseHUD();
    ballReleaseHUD->Draw(dT, camera, *gameModel);

    // Draw the remote control rocket HUD display
    RemoteControlRocketHUD* rocketHUD = gameAssets->GetRemoteControlRocketHUD();
    rocketHUD->Draw(dT, camera);

    // Draw the timers that are currently in existence
	gameAssets->DrawTimers(dT, camera, *gameModel);

	// Draw any HUD special elements based on currently active items, etc.
	gameAssets->DrawActiveItemHUDElements(dT, *gameModel, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	// The very last thing we do is draw the 'informative' game elements (e.g., tutorial stuff, or important information for the player)
	// this stuff should always be on the top
	gameAssets->DrawInformativeGameElements(camera, dT, *gameModel);

    debug_opengl_state();
}

void InGameRenderPipeline::RenderHUDWithAlpha(double dT, float alpha) {
    this->SetHUDAlpha(alpha);
    if (alpha > 0.0f) {
        this->RenderHUD(dT);
    }
}

void InGameRenderPipeline::SetHUDAlpha(float alpha) {
    GameAssets* gameAssets = this->display->GetAssets();

    gameAssets->GetBoostHUD()->SetAlpha(alpha);
    gameAssets->GetLifeHUD()->SetAlpha(alpha);
    gameAssets->GetPointsHUD()->SetAlpha(alpha);

    this->SetSpecialLabelsAlpha(alpha);
}