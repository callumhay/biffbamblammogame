
#include "InGameRenderPipeline.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameFontAssetsManager.h"
#include "LivesLeftHUD.h"

#include "../BlammoEngine/Camera.h"
#include "../GameModel/GameModel.h"

const unsigned int InGameRenderPipeline::HUD_X_INDENT = 10;	
const unsigned int InGameRenderPipeline::HUD_Y_INDENT = 10;

InGameRenderPipeline::InGameRenderPipeline(GameDisplay* display) : display(display) {
	assert(display != NULL);

	// Set HUD display elements
	float dropShadowAmt = 0.05f;
	Colour shadowColourHUD(0, 0, 0);
	Colour textColourHUD(1, 1, 1);
	// Score display
	this->scoreLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), "0");
	this->scoreLabel.SetColour(textColourHUD);
	this->scoreLabel.SetDropShadow(shadowColourHUD, dropShadowAmt);
}

InGameRenderPipeline::~InGameRenderPipeline() {
}
	
void InGameRenderPipeline::RenderFrameWithoutHUD(double dT) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Tick the assets (update them for amount of elapsed time dT).
	this->display->GetAssets()->Tick(dT);

	this->ApplyInGameCamera(dT);
	FBObj* backgroundFBO = this->RenderBackgroundToFBO(dT);
	this->RenderForegroundToFBO(backgroundFBO, dT);
	this->RenderFinalGather(dT);
}

void InGameRenderPipeline::RenderFrame(double dT) {
	this->RenderFrameWithoutHUD(dT);
	this->RenderHUD(dT);
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
	camera.ApplyCameraTransform(dT);
}

// Render just the background (includes the skybox, background geometry and effects), the rendering
// will be done to the background FBO in the FBO assets and a pointer to it will be returned
// NOTE: The caller does NOT assume ownership of the returned memory
FBObj* InGameRenderPipeline::RenderBackgroundToFBO(double dT) {
	UNUSED_PARAMETER(dT);

	Vector2D negHalfLevelDim = -0.5f * this->display->GetModel()->GetLevelUnitDimensions();
	
	// Attach the background FBO
	FBObj* backgroundFBO = this->display->GetAssets()->GetFBOAssets()->GetBackgroundFBO();
	assert(backgroundFBO != NULL);

	backgroundFBO->BindFBObj();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Draw the background of the current scene
	glPushMatrix();
	glTranslatef(0.0f, negHalfLevelDim[1], 0.0f);

	this->display->GetAssets()->DrawSkybox(this->display->GetCamera());
	this->display->GetAssets()->DrawBackgroundModel(this->display->GetCamera());
	this->display->GetAssets()->DrawBackgroundEffects(this->display->GetCamera());

	glPopMatrix();

	// Unbind the background FBO
	backgroundFBO->UnbindFBObj();

	debug_opengl_state();
	return backgroundFBO;
}

FBObj* InGameRenderPipeline::RenderForegroundToFBO(FBObj* backgroundFBO, double dT) {
	assert(backgroundFBO != NULL);

	const Camera& camera = this->display->GetCamera();
	Vector2D negHalfLevelDim = -0.5 * this->display->GetModel()->GetLevelUnitDimensions();
	GameFBOAssets* fboAssets = this->display->GetAssets()->GetFBOAssets();
	FBObj* postFullSceneFBO = fboAssets->GetPostFullSceneFBO();
	FBObj* fullSceneFBO = fboAssets->GetFullSceneFBO();
	
	assert(postFullSceneFBO != NULL);
	assert(fullSceneFBO != NULL);

	fullSceneFBO->BindFBObj();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	backgroundFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);

	glPushMatrix();
	Matrix4x4 gameTransform = this->display->GetModel()->GetTransformInfo()->GetGameTransform();
	glMultMatrixf(gameTransform.begin());

	// Tesla lightning arcs
	this->display->GetAssets()->DrawTeslaLightning(dT, camera);

	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);
	// Paddle...
	this->display->GetAssets()->DrawPaddle(dT, *this->display->GetModel()->GetPlayerPaddle(), camera);
	glPopMatrix();

	// Level pieces
	const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
	this->display->GetAssets()->DrawLevelPieces(dT, currLevel, camera);

	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Balls...
	this->display->GetAssets()->DrawGameBalls(dT, *this->display->GetModel(), camera, negHalfLevelDim);

	// Projectiles...
	this->display->GetAssets()->DrawProjectiles(dT, *this->display->GetModel(), camera);

	glPopMatrix();

	// Safety net (if active)
	this->display->GetAssets()->DrawSafetyNetIfActive(dT, camera);

	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Draw the dropping items if not in the last pass
	if (!fboAssets->DrawItemsInLastPass()) {
		std::list<GameItem*>& gameItems = this->display->GetModel()->GetLiveItems();
		for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); ++iter) {
			this->display->GetAssets()->DrawItem(dT, camera, (**iter));
		}				
	}

	// Render the beam effects
	this->display->GetAssets()->DrawBeams(*this->display->GetModel(), camera);

	// Draw Post-Fullscene effects
	postFullSceneFBO->BindFBObj();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	fullSceneFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1);
	
	// Render any post-processing effects for various items/objects in the game
	this->display->GetAssets()->DrawPaddlePostEffects(dT, *this->display->GetModel(), camera);
	this->display->GetAssets()->DrawStatusEffects(dT, camera);

	FBObj::UnbindFBObj();

	glPopMatrix();

	// Do a gaussian blur for a softer feeling
	this->display->GetAssets()->GetFBOAssets()->RenderFullSceneBlur(camera.GetWindowWidth(), camera.GetWindowHeight(), dT);

	debug_opengl_state();

	return postFullSceneFBO;
}

void InGameRenderPipeline::RenderFinalGather(double dT) {
	GameFBOAssets* fboAssets = this->display->GetAssets()->GetFBOAssets();
	GameModel* gameModel = this->display->GetModel();
	const Camera& camera = this->display->GetCamera();

	// Render fullscreen effects 
	FBObj* initialFBO = fboAssets->RenderInitialFullscreenEffects(camera.GetWindowWidth(), camera.GetWindowHeight(), dT);
	assert(initialFBO != NULL);

	FBObj* finalFBO = fboAssets->GetFinalFullScreenFBO();
	assert(finalFBO != NULL);

	// Final non-fullscreen draw pass - draw the falling items and particles
	finalFBO->BindFBObj();
	initialFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);

	// Render all effects that do not go through all the post-processing filters...
	Vector3D negHalfLevelDim = Vector3D(-0.5 * gameModel->GetLevelUnitDimensions(), 0.0);
	glPushMatrix();
	Matrix4x4 gameTransform = gameModel->GetTransformInfo()->GetGameTransform();
	glMultMatrixf(gameTransform.begin());
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], negHalfLevelDim[2]);
	
	// Draw the dropping items if in the last pass
	if (fboAssets->DrawItemsInLastPass()) {
		std::list<GameItem*>& gameItems = gameModel->GetLiveItems();
		for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); ++iter) {
			this->display->GetAssets()->DrawItem(dT, camera, (**iter));
		}			
	}

	// Typical Particle effects...
	GameESPAssets* espAssets = this->display->GetAssets()->GetESPAssets();
	espAssets->DrawBeamEffects(dT, camera, negHalfLevelDim);
	espAssets->DrawParticleEffects(dT, camera);

	// Absolute post effects call for various object effects
	this->display->GetAssets()->DrawGameBallsPostEffects(dT, *gameModel, camera);

	finalFBO->UnbindFBObj();

	// Render the final fullscreen effects
	fboAssets->RenderFinalFullscreenEffects(camera.GetWindowWidth(), camera.GetWindowHeight(), dT, *gameModel);

	glPopMatrix();
	debug_opengl_state();
}

void InGameRenderPipeline::RenderHUD(double dT) {
	GameModel* gameModel = this->display->GetModel();
	const Camera& camera = this->display->GetCamera();

	const int DISPLAY_WIDTH  = camera.GetWindowWidth();
	const int DISPLAY_HEIGHT = camera.GetWindowHeight();

	// Draw the points in the top-right corner of the display
	std::stringstream ptStrStream;
	ptStrStream << gameModel->GetScore();
	this->scoreLabel.SetText(ptStrStream.str());
	this->scoreLabel.SetTopLeftCorner(Point2D(DISPLAY_WIDTH - HUD_X_INDENT - this->scoreLabel.GetLastRasterWidth(), DISPLAY_HEIGHT - HUD_Y_INDENT));
	this->scoreLabel.Draw();

	// Draw the number of lives left in the top-left corner of the display
	this->display->GetAssets()->GetLifeHUD()->Draw(dT, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	
	// Draw the timers that are currently in existance
	this->display->GetAssets()->DrawTimers(dT, camera);

	// Draw any HUD special elements based on currently active items, etc.
	this->display->GetAssets()->DrawActiveItemHUDElements(dT, *gameModel, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	// The very last thing we do is draw the 'informative' game elements (e.g., tutorial stuff, or important information for the player)
	// this stuff should always be on the top
	this->display->GetAssets()->DrawInformativeGameElements(camera, dT, *gameModel);

	debug_opengl_state();
}