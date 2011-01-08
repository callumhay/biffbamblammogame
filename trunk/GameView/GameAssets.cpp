/**
 * GameAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameAssets.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "CgFxPostRefract.h"
#include "CgFxVolumetricEffect.h"
#include "CgFxFireBallEffect.h"
#include "GameFontAssetsManager.h"
#include "LoadingScreen.h"
#include "LivesLeftHUD.h"
#include "CrosshairLaserHUD.h"
#include "PlayerHurtHUD.h"
#include "FlashHUD.h"
#include "StickyPaddleGoo.h"
#include "LaserPaddleGun.h"
#include "PaddleRocketMesh.h"
#include "PaddleShield.h"

// Game Model includes
#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/Beam.h"
#include "../GameModel/PaddleRocketProjectile.h"
#include "../GameModel/FireGlobProjectile.h"

// Game Control Includes
#include "../GameControl/GameControllerManager.h"

// Blammo Engine includes
#include "../BlammoEngine/Texture3D.h"

// ESP Engine includes
#include "../ESPEngine/ESPEmitter.h"

// Basic Top Level Includes
#include "../ResourceManager.h"

// *****************************************************

GameAssets::GameAssets(int screenWidth, int screenHeight): 

currentLevelMesh(NULL),
worldAssets(NULL),
espAssets(NULL),
itemAssets(NULL),
fboAssets(NULL),
lightAssets(NULL),
soundAssets(NULL),

lifeHUD(NULL),
crosshairHUD(NULL),
painHUD(NULL),
flashHUD(NULL),

ball(NULL), 
spikeyBall(NULL),
rocketMesh(NULL),
paddleBeamAttachment(NULL),
paddleLaserAttachment(NULL),
paddleStickyAttachment(NULL),
paddleShield(NULL),

invisiBallEffect(NULL), 
ghostBallEffect(NULL),
fireBallEffect(NULL)
{

	// Load ESP assets
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading purdy pictures...");
	this->espAssets = new GameESPAssets();

	// Load FBO assets
	this->fboAssets = new GameFBOAssets(screenWidth, screenHeight);

	// Load item assets
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading game items...");
	this->itemAssets = new GameItemAssets(this->espAssets);
	bool didItemAssetsLoad = this->itemAssets->LoadItemAssets();
	assert(didItemAssetsLoad);

	// Load basic default in-memory sounds
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading melodic tunage...");
	this->soundAssets = new GameSoundAssets();
	this->soundAssets->LoadSoundPallet(GameSoundAssets::MainMenuSoundPallet);

	// Load all fonts
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading fonts...");
	GameFontAssetsManager::GetInstance()->LoadMinimalFonts();

	// Load regular meshes
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading geometry...");
	this->LoadRegularMeshAssets();

	// Load regular effects
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading effects...");
	this->LoadRegularEffectAssets();

	// Initialize any HUD elements
	this->crosshairHUD	= new CrosshairLaserHUD();
	this->lifeHUD			  = new LivesLeftHUD();
	this->painHUD				= new PlayerHurtHUD();
	this->flashHUD			= new FlashHUD();

	// Initialize the light assets
	this->lightAssets = new GameLightAssets();
}

GameAssets::~GameAssets() {
	// Delete regular effect assets
	this->DeleteRegularEffectAssets();

	// Delete the currently loaded world and level assets if there are any
	// Delete all the levels for the world that are currently loaded - THIS MUST BE CALLED BEFORE DELETING
	// THE WORLD ASSETS!!!
	if (this->currentLevelMesh != NULL) {
		delete this->currentLevelMesh;
		this->currentLevelMesh = NULL;
	}

	if (this->worldAssets != NULL) {
		delete this->worldAssets;
		this->worldAssets = NULL;
	}

	// Delete item assets
	if (this->itemAssets != NULL) {
		delete this->itemAssets;
		this->itemAssets = NULL;
	}

	bool success = ResourceManager::GetInstance()->ReleaseMeshResource(this->paddleBeamAttachment);
	assert(success);

	delete this->rocketMesh;
	this->rocketMesh = NULL;

	delete this->paddleLaserAttachment;
	this->paddleLaserAttachment = NULL;
	delete this->paddleStickyAttachment;
	this->paddleStickyAttachment = NULL;
	delete this->paddleShield;
	this->paddleShield = NULL;

	// Clear up the FrameBuffer Object assets
	delete this->fboAssets;
	this->fboAssets = NULL;

	// Clear up the light assets
	delete this->lightAssets;
	this->lightAssets = NULL;

	// Clean up sound assets
	delete this->soundAssets;
	this->soundAssets = NULL;

	// Delete any HUD objects
	delete this->lifeHUD;
	this->lifeHUD = NULL;
	delete this->crosshairHUD;
	this->crosshairHUD = NULL;
	delete this->painHUD;
	this->painHUD = NULL;
	delete this->flashHUD;
	this->flashHUD = NULL;
}


// Draw the game's ball (the thing that bounces and blows stuff up), position it, 
// draw the materials and draw the mesh.
void GameAssets::DrawGameBalls(double dT, GameModel& gameModel, const Camera& camera, const Vector2D& worldT) {
	
	// Average values used to calculate the colour and position of the ball light
	Point3D avgBallPosition(0,0,0);
	Colour avgBallColour(0,0,0);
	unsigned int visibleBallCount = 0;

	// Go through each ball in the game, draw it accordingly
	const std::list<GameBall*>& balls = gameModel.GetGameBalls();
	for (std::list<GameBall*>::const_iterator ballIter = balls.begin(); ballIter != balls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		
		bool ballIsInvisible = (currBall->GetBallType() & GameBall::InvisiBall) == GameBall::InvisiBall;
		CgFxEffectBase* ballEffectTemp = NULL;
		Colour currBallColour(0,0,0);

		Point3D ballPos = currBall->GetCenterPosition();

		if (currBall->GetBallType() == GameBall::NormalBall) {
			// Normal ball with a regular light
			avgBallPosition = avgBallPosition + Vector3D(ballPos[0], ballPos[1], ballPos[2]);
			avgBallColour = avgBallColour + GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_COLOUR;
			visibleBallCount++;
		}
		else {
			// The ball has an item in effect on it... figure out what effect(s) and render it/them appropriately

			unsigned int numColoursApplied = 0;
			
			// GHOST BALL CHECK
			if ((currBall->GetBallType() & GameBall::GhostBall) == GameBall::GhostBall && !ballIsInvisible) {
				
				// Draw when the ghost ball is not an invisiball...	
				ballEffectTemp = this->ghostBallEffect;

				// We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
					this->espAssets->DrawGhostBallEffects(dT, camera, *currBall);
				}
				
				currBallColour = currBallColour + GameModelConstants::GetInstance()->GHOST_BALL_COLOUR;
				numColoursApplied++;
			}

			// UBER BALL CHECK
			if ((currBall->GetBallType() & GameBall::UberBall) == GameBall::UberBall && !ballIsInvisible) {
				
				// Draw when uber ball and not invisiball...
				// We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
					this->espAssets->DrawUberBallEffects(dT, camera, *currBall);
				}

				currBallColour = currBallColour + GameModelConstants::GetInstance()->UBER_BALL_COLOUR;
				numColoursApplied++;
			}

			// GRAVITY BALL CHECK
			if ((currBall->GetBallType() & GameBall::GraviBall) == GameBall::GraviBall && !ballIsInvisible) {

				// Draw when gravity ball and not invisiball...
				// We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
					Vector3D gravityDir = gameModel.GetTransformInfo()->GetGameTransform() * Vector3D(0, -1, 0);
					this->espAssets->DrawGravityBallEffects(dT, camera, *currBall, gravityDir);
				}
				
				currBallColour = currBallColour + GameModelConstants::GetInstance()->GRAVITY_BALL_COLOUR;
				numColoursApplied++;
			}

			// CRAZY BALL CHECK
			if ((currBall->GetBallType() & GameBall::CrazyBall) == GameBall::CrazyBall && !ballIsInvisible) {

				// Draw when crazy ball when not invisible...
				// We don't draw any of the effects if we're in ball camera mode
				if (!GameBall::GetIsBallCameraOn()) {				
					this->espAssets->DrawCrazyBallEffects(dT, camera, *currBall);
				}
				
				currBallColour = currBallColour + GameModelConstants::GetInstance()->CRAZY_BALL_COLOUR;
				numColoursApplied++;
			}

			// FIRE BALL CHECK
			if ((currBall->GetBallType() & GameBall::FireBall) == GameBall::FireBall && !ballIsInvisible) {
				ballEffectTemp = this->fireBallEffect;

				// Draw the fire ball when not invisible
				// We don't draw any of the effects if we're in a ball camera mode or inside a cannon
				if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {				
					this->espAssets->DrawFireBallEffects(dT, camera, *currBall);
				}
				currBallColour = currBallColour + GameModelConstants::GetInstance()->FIRE_BALL_COLOUR;
				numColoursApplied++;
			}

			// ICE BALL CHECK
			if ((currBall->GetBallType() & GameBall::IceBall) == GameBall::IceBall && !ballIsInvisible) {
				//ballEffectTemp = this->iceBallEffect;

				// Draw the ice ball when not invisible
				// We don't draw any of the effects if we're in a ball camera mode or inside a cannon
				if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {				
					this->espAssets->DrawIceBallEffects(dT, camera, *currBall);
				}
				currBallColour = currBallColour + GameModelConstants::GetInstance()->ICE_BALL_COLOUR;
				numColoursApplied++;
			}

			// INVISIBALL CHECK
			if (ballIsInvisible) {
				// Obtain the POST full screen rendering from the previous frame - this is
				// a bit of a hack but it saves us from reading/writing to the same FBO simultaneously
				this->invisiBallEffect->SetFBOTexture(this->fboAssets->GetPostFullSceneFBO()->GetFBOTexture());
				ballEffectTemp = this->invisiBallEffect;
			}
			else {
				// We only take the average of visible balls.
				avgBallPosition = avgBallPosition + Vector3D(ballPos[0], ballPos[1], ballPos[2]);
			
				numColoursApplied = std::max<int>(1, numColoursApplied);
				avgBallColour = avgBallColour + (currBallColour / numColoursApplied);

				visibleBallCount++;
			}
		}

		// Draw the ball model...
		
		// Obtain the colour of the ball from the model, if it's completely invisible
		// then just skip the rest of the draw calls
		const ColourRGBA& ballColour = currBall->GetColour();
		if (ballColour.A() < EPSILON) {
			continue;
		}

		glPushMatrix();
		float ballScaleFactor = currBall->GetBallScaleFactor();
		glTranslatef(ballPos[0], ballPos[1], ballPos[2]);
		
		// Draw background effects for the ball
		this->espAssets->DrawBackgroundBallEffects(dT, camera, *currBall);

		Vector3D ballRot = currBall->GetRotation();
		glRotatef(ballRot[0], 1.0f, 0.0f, 0.0f);
		glRotatef(ballRot[1], 0.0f, 1.0f, 0.0f);
		glRotatef(ballRot[2], 0.0f, 0.0f, 1.0f);
		glScalef(ballScaleFactor, ballScaleFactor, ballScaleFactor);
		// Always make the colour of the invisiball to be plain white - we don't want to affect
		// the cloaking effect
		if (ballIsInvisible) {
			glColor4f(1.0f, 1.0f, 1.0f, ballColour.A());
		}
		else {
			glColor4f(ballColour.R(), ballColour.G(), ballColour.B(), ballColour.A());
		}

		BasicPointLight ballKeyLight, ballFillLight;
		this->lightAssets->GetBallAffectingLights(ballKeyLight, ballFillLight);

		if ((currBall->GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
			this->spikeyBall->Draw(camera, ballEffectTemp, ballKeyLight, ballFillLight);
		}
		else {
			this->ball->Draw(camera, ballEffectTemp, ballKeyLight, ballFillLight);
		}

		glPopMatrix();
	}

	// Calculate the average position and colour of all the visible balls in the game
	if (visibleBallCount > 0) {
		avgBallPosition = avgBallPosition / visibleBallCount;
		avgBallPosition = avgBallPosition + Vector3D(worldT[0], worldT[1], 0.0f);
		avgBallColour = avgBallColour / visibleBallCount;
		
		// Grab a trasform matrix from the game model to say where the ball light is
		// if the level is flipped or some such thing
		Point3D newAvgBallPos =  gameModel.GetTransformInfo()->GetGameTransform() * avgBallPosition;

		// Set the ball light to the correct position
		this->lightAssets->GetBallLight().SetPosition(newAvgBallPos);
	}
	else {
		avgBallColour = Colour(0,0,0);
	}

	// Set the ball light to the correct diffuse colour
	this->lightAssets->GetBallLight().SetDiffuseColour(avgBallColour);
	debug_opengl_state();
}

/**
 * Draw the effects that take place after drawing everything (Except final fullscreen effects).
 */
void GameAssets::DrawGameBallsPostEffects(double dT, GameModel& gameModel, const Camera& camera) {
	
	// Go through each ball and draw its post effects
	const std::list<GameBall*>& balls = gameModel.GetGameBalls();

	for (std::list<GameBall*>::const_iterator ballIter = balls.begin(); ballIter != balls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;		
		
		// PADDLE CAMERA CHECK
		PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
		if (paddle->GetIsPaddleCameraOn() && (currBall->GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall &&
			  !gameModel.IsBlackoutEffectActive()) {
			// Draw a target around balls when in paddle camera mode AND ball is not invisible
			this->espAssets->DrawPaddleCamEffects(dT, camera, *currBall, *paddle);
		}
	}

	debug_opengl_state();
}

void GameAssets::Tick(double dT) {
	// Tick the world assets (e.g., for background animations)
	this->worldAssets->Tick(dT);

	// Tick the FBO assets (e.g., for post-processing / fullscreen effects animations)
	this->fboAssets->Tick(dT);

	// Tick the light assets (light animations for strobing, changing colours, etc.)
	this->lightAssets->Tick(dT);

	// Update the esp assets with any changes over the last tick
	this->espAssets->UpdateBGTexture(*this->fboAssets->GetFinalFullScreenFBO()->GetFBOTexture());
}

/**
 * Draw the player paddle mesh with materials and in correct position.
 */
void GameAssets::DrawPaddle(double dT, const PlayerPaddle& p, const Camera& camera) {
	const Point2D& paddleCenter = p.GetCenterPosition();	
	float paddleScaleFactor = p.GetPaddleScaleFactor();
	float scaleHeightAdjustment = PlayerPaddle::PADDLE_HALF_HEIGHT * (paddleScaleFactor - 1);
	
	// Obtain the lights that affect the paddle
	BasicPointLight paddleKeyLight, paddleFillLight, ballLight;
	this->lightAssets->GetPaddleAffectingLights(paddleKeyLight, paddleFillLight, ballLight);

	glPushMatrix();
	glTranslatef(paddleCenter[0], paddleCenter[1] + scaleHeightAdjustment, 0);

	if (!p.GetIsPaddleCameraOn()) {
		// Draw the shield around the paddle (if it's currently activated)
		this->paddleShield->DrawAndTick(p, camera, dT);
	}

	// Draw any effects on the paddle (e.g., item acquiring effects)
	this->espAssets->DrawBackgroundPaddleEffects(dT, camera);
	// Draw the paddle
	this->worldAssets->DrawPaddle(p, camera, paddleKeyLight, paddleFillLight, ballLight);

	if ((p.GetPaddleType() & PlayerPaddle::LaserBeamPaddle) == PlayerPaddle::LaserBeamPaddle) {
		if (p.GetIsLaserBeamFiring()) {
			// Draw paddle blasty stuff when firing
			if (!p.GetIsPaddleCameraOn()) {
				this->espAssets->DrawPaddleLaserBeamFiringEffects(dT, camera, p);
			}
		}
		// Beam isn't firing yet, if the rocket is on the paddle then don't draw any of the beam effects
		// coming out of the paddle
		else if ((p.GetPaddleType() & PlayerPaddle::RocketPaddle) != PlayerPaddle::RocketPaddle){
			// Draw glowy beam origin when beam is able to fire but not actually firing yet
			this->espAssets->DrawPaddleLaserBeamBeforeFiringEffects(dT, camera, p);
		}
	}

	// In the case of a laser bullet paddle (and NOT paddle camera mode), we draw the laser attachment and its related effects
	// Camera mode is exempt from this because the attachment would seriously get in the view of the player
	if (!p.GetIsPaddleCameraOn()) {
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		if ((p.GetPaddleType() & PlayerPaddle::LaserBulletPaddle) == PlayerPaddle::LaserBulletPaddle) {
			// Draw attachment (gun) mesh
			this->paddleLaserAttachment->Draw(dT, p, camera, paddleKeyLight, paddleFillLight);
		}
		if ((p.GetPaddleType() & PlayerPaddle::LaserBeamPaddle) == PlayerPaddle::LaserBeamPaddle) {
			glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
			this->paddleBeamAttachment->Draw(camera, paddleKeyLight, paddleFillLight);
		}
	}

	glPopMatrix();
	debug_opengl_state();
}

/**
 * Used to draw any relevant post-processing-related effects for the paddle.
 */
void GameAssets::DrawPaddlePostEffects(double dT, GameModel& gameModel, const Camera& camera) {
	PlayerPaddle* paddle = gameModel.GetPlayerPaddle();

	// Do nothing if we are in paddle camera mode
	if (paddle->GetIsPaddleCameraOn()) {
		return;
	}
	
	const Point2D& paddleCenter = paddle->GetCenterPosition();
	float paddleScaleFactor = paddle->GetPaddleScaleFactor();
	float scaleHeightAdjustment = PlayerPaddle::PADDLE_HALF_HEIGHT * (paddleScaleFactor - 1);

	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	glTranslatef(paddleCenter[0], paddleCenter[1] + scaleHeightAdjustment, 0);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	

	// When the paddle has the 'sticky' power-up we attach sticky goo to its top
	if ((paddle->GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {	
		// Set the texture for the refraction in the goo - be careful here, we can't get
		// the 'post full scene' fbo because we are currently in the middle of drawing to it
		this->paddleStickyAttachment->SetSceneTexture(this->fboAssets->GetFullSceneFBO()->GetFBOTexture());

		// In the case where the paddle laser beam is also active then the beam will refract through the goo,
		// illuminate the goo alot more
		if ((paddle->GetPaddleType() & PlayerPaddle::LaserBeamPaddle) == PlayerPaddle::LaserBeamPaddle && paddle->GetIsLaserBeamFiring()) {
			this->paddleStickyAttachment->SetPaddleLaserBeamIsActive(true);
		}
		else {
			this->paddleStickyAttachment->SetPaddleLaserBeamIsActive(false);
		}

		// Obtain the lights that affect the paddle
		BasicPointLight paddleKeyLight, paddleFillLight, ballLight;
		this->lightAssets->GetPaddleAffectingLights(paddleKeyLight, paddleFillLight, ballLight);

		// Draw the sticky goo
		this->paddleStickyAttachment->Draw(*paddle, camera, paddleKeyLight, paddleFillLight, ballLight);
	}

	// In order to draw the laser glow effect make sure there isn't a rocket sitting on the paddle
	if ((paddle->GetPaddleType() & PlayerPaddle::LaserBulletPaddle) == PlayerPaddle::LaserBulletPaddle &&
		  (paddle->GetPaddleType() & PlayerPaddle::RocketPaddle) != PlayerPaddle::RocketPaddle) {
		// Draw glowy effects where the laser originates...
		this->espAssets->DrawPaddleLaserBulletEffects(dT, camera, *paddle);
	}

	glPopMatrix();

	// BALL CAMERA CHECK
	if (GameBall::GetIsBallCameraOn() && !gameModel.IsBlackoutEffectActive()) {
		const GameBall* ballWithCam = GameBall::GetBallCameraBall();
		// Draw a target around balls when in paddle camera mode AND ball is not invisible
		this->espAssets->DrawBallCamEffects(dT, camera, *ballWithCam, *paddle);
	}
	glPopAttrib();

	debug_opengl_state();
}

/**
 * Draw the skybox for the current world type.
 */
void GameAssets::DrawSkybox(const Camera& camera) {
	this->worldAssets->DrawSkybox(camera);
}

/**
 * Draw the background model for the current world type.
 */
void GameAssets::DrawBackgroundModel(const Camera& camera) {
	BasicPointLight bgKeyLight, bgFillLight;
	this->lightAssets->GetBackgroundAffectingLights(bgKeyLight, bgFillLight);
	this->worldAssets->DrawBackgroundModel(camera, bgKeyLight, bgFillLight);
}

/**
 * Draw the background effects for the current world type.
 */
void GameAssets::DrawBackgroundEffects(const Camera& camera) {
	this->worldAssets->DrawBackgroundEffects(camera);
}

/**
 * Draw a given item in the world.
 */
void GameAssets::DrawItem(double dT, const Camera& camera, const GameItem& gameItem) {
	BasicPointLight fgKeyLight, fgFillLight, ballLight;
	this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
	this->itemAssets->DrawItem(dT, camera, gameItem, fgKeyLight, fgFillLight, ballLight);
}

/**
 * Draw the HUD timer for the given timer type.
 */
void GameAssets::DrawTimers(double dT, const Camera& camera) {
	this->itemAssets->DrawTimers(dT, camera);
}

void GameAssets::DrawBeams(const GameModel& gameModel, const Camera& camera) {
	const std::list<Beam*>& beams = gameModel.GetActiveBeams();
	if (beams.empty()) {
		return;
	}

	const float TYPICAL_BEAM_ALPHA = 0.4f;
	const Colour& beamColour = GameViewConstants::GetInstance()->LASER_BEAM_COLOUR;
	const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
	float quarterPaddleDepth = paddle->GetHalfDepthTotal() / 2.0f;

	glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// If in paddle cam mode draw a fullscreen quad with the laser colour
	if (paddle->GetIsPaddleCameraOn()) {
		GeometryMaker::GetInstance()->DrawFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 0.0, ColourRGBA(beamColour, 0.2f));
	}

	glPushMatrix();
	glTranslatef(0, 0, 0);
	glBegin(GL_QUADS);
	Point3D temp;
	Point3D beamSegStart, beamSegEnd;
	Vector3D beamRightVec;
	Vector3D beamUpVec;
	Vector3D beamDepthVec;
	float currRadius;
	Vector2D tempRadiusOffset;
	
	for (std::list<Beam*>::const_iterator beamIter = beams.begin(); beamIter != beams.end(); ++beamIter) {
		const Beam* currentBeam = *beamIter;

		const std::list<BeamSegment*>& beamSegments = currentBeam->GetBeamParts();
		std::list<BeamSegment*>::const_iterator segmentIter = beamSegments.begin();
		
		int segCounter = 0;
		const int NUM_BASE_SEGMENTS = currentBeam->GetNumBaseBeamSegments();

		for (; segmentIter != beamSegments.end(); ++segmentIter, ++segCounter) {

			if (paddle->GetIsPaddleCameraOn() && segCounter < NUM_BASE_SEGMENTS) {
				continue;
			}

			const BeamSegment* currentSeg = *segmentIter;

			beamSegStart = Point3D(currentSeg->GetStartPoint());
			beamSegEnd   = Point3D(currentSeg->GetEndPoint());
			beamUpVec    = Vector3D(currentSeg->GetBeamSegmentRay().GetUnitDirection());

			// Center of the beam (brighter in the center) 
			currRadius   = 0.5f * currentSeg->GetRadius();
			beamRightVec = currRadius * Vector3D(beamUpVec[1], -beamUpVec[0], 0);
			beamDepthVec = Vector3D(0, 0, 0.5f * quarterPaddleDepth);

			glColor4f(0.75f, 1.0f, 1.0f, TYPICAL_BEAM_ALPHA);
			
			// Front face
			temp = beamSegStart - beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegStart + beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd + beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd - beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());

			// Back face
			temp = beamSegStart - beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegStart + beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd + beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd - beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());

			// Right face
			temp = beamSegStart + beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegStart + beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd + beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd + beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());

			// Left face
			temp = beamSegStart - beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegStart - beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd - beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd - beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());

			// We want the first beams to fill up the entire screen in paddle cam mode
			currRadius   = currentSeg->GetRadius();
			beamRightVec = currRadius * Vector3D(beamUpVec[1], -beamUpVec[0], 0);
			beamDepthVec = Vector3D(0, 0, quarterPaddleDepth);

			glColor4f(beamColour.R(), beamColour.G(), beamColour.B(), TYPICAL_BEAM_ALPHA);

			// Front face
			temp = beamSegStart - beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegStart + beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd + beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd - beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());

			// Back face
			temp = beamSegStart - beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegStart + beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd + beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd - beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());

			// Right face
			temp = beamSegStart + beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegStart + beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd + beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd + beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());

			// Left face
			temp = beamSegStart - beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegStart - beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd - beamRightVec - beamDepthVec;
			glVertex3fv(temp.begin());
			temp = beamSegEnd - beamRightVec + beamDepthVec;
			glVertex3fv(temp.begin());
		}
	}
	glEnd();
	glPopMatrix();
	glPopAttrib();

	debug_opengl_state();
}

/**
 * Draw the currently active projectiles in the game.
 */
void GameAssets::DrawProjectiles(double dT, const GameModel& gameModel, const Camera& camera) {
	// Get lights affecting the foreground meshes...
	BasicPointLight keyLight, fillLight, ballLight;
	this->lightAssets->GetPieceAffectingLights(keyLight, fillLight, ballLight);

	this->rocketMesh->Draw(dT, *gameModel.GetPlayerPaddle(), camera, keyLight, fillLight, ballLight);
	this->espAssets->DrawProjectileEffects(dT, camera);
}

/**
 * Draw informational game elements (e.g., tutorial bubbles, information about items acquired, etc.).
 */
void GameAssets::DrawInformativeGameElements(const Camera& camera, double dT, const GameModel& gameModel) {
	if (!this->randomToItemAnimation.GetIsActive()) {
		return;
	}

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);

	Vector3D negHalfLevelDim = Vector3D(-0.5 * gameModel.GetLevelUnitDimensions(), 0.0);
	glPushMatrix();
	Matrix4x4 gameTransform = gameModel.GetTransformInfo()->GetGameTransform();
	glMultMatrixf(gameTransform.begin());
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], negHalfLevelDim[2]);

	// Draw the random item to actual item animation when required...
	this->randomToItemAnimation.Draw(camera, dT);

	glPopMatrix();

	glPopAttrib();

}

/**
 * Draw HUD elements for any of the active items when applicable.
 */
void GameAssets::DrawActiveItemHUDElements(double dT, const GameModel& gameModel, int displayWidth, int displayHeight) {
	const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
	assert(paddle != NULL);

	// When in paddle camera mode, the user will see the effects currently in the paddle from a first-person
	// perspective, we need to make these a bit more presentable...
	if (paddle->GetIsPaddleCameraOn()) {
		this->crosshairHUD->Tick(dT);
		this->crosshairHUD->Draw(paddle, displayWidth, displayHeight, (1.0 - paddle->GetColour().A()));
	}


	// Draw any pain overlay when active
	this->painHUD->Draw(dT, displayWidth, displayHeight);
	// Draw any flashes / fades
	this->flashHUD->Draw(dT, displayWidth, displayHeight);
}

void GameAssets::LoadNewLevelMesh(const GameLevel& currLevel) {
	if (this->currentLevelMesh != NULL) {
		// If the level mesh object is already loaded then use it's proper method - this
		// avoids reloading a whole bunch of common assets
		this->currentLevelMesh->LoadNewLevel(*this->worldAssets, *this->itemAssets, currLevel);
	}
	else {
		this->currentLevelMesh = new LevelMesh(*this->worldAssets, *this->itemAssets, currLevel);
	}
}

void GameAssets::LoadRegularMeshAssets() {
	if (this->ball == NULL) {
		this->ball = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BALL_MESH);
	}
	if (this->spikeyBall == NULL) {
		this->spikeyBall = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SPIKEY_BALL_MESH);
	}
	if (this->rocketMesh == NULL) {
		this->rocketMesh = new PaddleRocketMesh();
	}
	if (this->paddleBeamAttachment == NULL) {
		this->paddleBeamAttachment = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_BEAM_ATTACHMENT_MESH);
	}
	if (this->paddleLaserAttachment == NULL) {
		this->paddleLaserAttachment = new LaserPaddleGun();
	}
	if (this->paddleStickyAttachment == NULL) {
		this->paddleStickyAttachment = new StickyPaddleGoo();
	}
	
}

void GameAssets::LoadRegularEffectAssets() {
	if (this->invisiBallEffect == NULL) {
		this->invisiBallEffect = new CgFxPostRefract();
		this->invisiBallEffect->SetWarpAmountParam(50.0f);
		this->invisiBallEffect->SetIndexOfRefraction(1.33f);
	}
	if (this->ghostBallEffect == NULL) {
		this->ghostBallEffect = new CgFxVolumetricEffect();
		this->ghostBallEffect->SetTechnique(CgFxVolumetricEffect::GHOSTBALL_TECHNIQUE_NAME);
		this->ghostBallEffect->SetColour(Colour(0.80f, 0.90f, 1.0f));
		this->ghostBallEffect->SetFadeExponent(1.0f);
		this->ghostBallEffect->SetScale(0.1f);
		this->ghostBallEffect->SetFrequency(0.8f);
		this->ghostBallEffect->SetAlphaMultiplier(1.0f);
		this->ghostBallEffect->SetFlowDirection(Vector3D(0, 0, 1));
	}
	if (this->fireBallEffect == NULL) {
		this->fireBallEffect = new CgFxFireBallEffect();
	}
	if (this->paddleShield == NULL) {
		this->paddleShield = new PaddleShield();
	}
}

void GameAssets::DeleteRegularEffectAssets() {
	// Delete the invisiball effect
	if (this->invisiBallEffect != NULL) {
		delete this->invisiBallEffect;
		this->invisiBallEffect = NULL;
	}

	if (this->ghostBallEffect != NULL) {
		delete this->ghostBallEffect;
		this->ghostBallEffect = NULL;
	}

	if (this->fireBallEffect != NULL) {
		delete this->fireBallEffect;
		this->fireBallEffect = NULL;
	}

	// Delete any left behind particles
	if (this->espAssets != NULL) {
		delete this->espAssets;
		this->espAssets = NULL;
	}
}

/**
 * Add the given projectile and create associated assets for it in the game assets.
 */
void GameAssets::AddProjectile(const GameModel& gameModel, const Projectile& projectile) {
	// Add the projectile's sprite/emitter effects
	this->espAssets->AddProjectileEffect(gameModel, projectile); 

	// Add any other view-related effects for the given projectile
	switch (projectile.GetType()) {
		case Projectile::PaddleLaserBulletProjectile:
			// Have the laser gun attachment move downwards in reaction to the laser being shot
			this->FirePaddleLaser(*gameModel.GetPlayerPaddle());
			break;
		case Projectile::PaddleRocketBulletProjectile:
			// Notify assets of the rocket...
			this->FirePaddleRocket(projectile);
			break;
		default:
			break;
	}
}

/** 
 * Remove the given projectile and its effects from the assets.
 */
void GameAssets::RemoveProjectile(const Projectile& projectile) {
	this->espAssets->RemoveProjectileEffect(projectile);

	switch (projectile.GetType()) {
		case Projectile::PaddleRocketBulletProjectile:
			this->rocketMesh->Deactivate(dynamic_cast<const PaddleRocketProjectile*>(&projectile));
			// Turn off the sound for the rocket mask and play the explosion sound
			this->soundAssets->StopWorldSound(GameSoundAssets::WorldSoundRocketMovingMask);
			break;
        
        case Projectile::FireGlobProjectile:
            this->espAssets->AddFireGlobDestroyedEffect(projectile);
            break;

		default:
			break;
	}
}

/**
 * Private helper, cause the laser paddle attachment to animate - like it's reacting to shooting
 * a bullet or something.
 */
void GameAssets::FirePaddleLaser(const PlayerPaddle& paddle) {
	this->paddleLaserAttachment->FirePaddleLaserGun(paddle);
}

/**
 * Private helper, causes the rocket to be fired and drawn.
 */
void GameAssets::FirePaddleRocket(const Projectile& rocketProjectile) {
	assert(rocketProjectile.GetType() == Projectile::PaddleRocketBulletProjectile);
	this->rocketMesh->Activate(dynamic_cast<const PaddleRocketProjectile*>(&rocketProjectile));
	this->soundAssets->PlayWorldSound(GameSoundAssets::WorldSoundRocketMovingMask);
}

/**
 * Notifies the assets that the paddle has been hurt so that the appropriate effects can
 * occur to show the hurting.
 */
void GameAssets::PaddleHurtByProjectile(const PlayerPaddle& paddle, const Projectile& projectile) {
	
	
	// Add the sprite/particle effect
	this->espAssets->AddPaddleHitByProjectileEffect(paddle, projectile);

	PlayerHurtHUD::PainIntensity intensity = PlayerHurtHUD::MinorPain;
	switch (projectile.GetType()) {
		case Projectile::CollateralBlockProjectile:
			intensity = PlayerHurtHUD::MajorPain;
			break;
		case Projectile::PaddleLaserBulletProjectile:
			intensity = PlayerHurtHUD::MinorPain;
			break;
		case Projectile::PaddleRocketBulletProjectile:
			intensity = PlayerHurtHUD::MajorPain;
			break;
		case Projectile::FireGlobProjectile: {
				const FireGlobProjectile* fireGlobProjectile = static_cast<const FireGlobProjectile*>(&projectile);
				switch (fireGlobProjectile->GetRelativeSize()) {
					case FireGlobProjectile::Small:
						intensity = PlayerHurtHUD::MinorPain;
						break;
					case FireGlobProjectile::Medium:
						intensity = PlayerHurtHUD::ModeratePain;
						break;
					case FireGlobProjectile::Large:
						intensity = PlayerHurtHUD::MajorPain;
						break;
					default:
						assert(false);
						break;
				}
			}
			break;
		default:
			assert(false);
			return;
	}

	// Add a fullscreen overlay effect to show pain/badness
	this->painHUD->Activate(intensity);

}

// Notifies assets that there was an explosion and there should be a fullscreen flash
void GameAssets::RocketExplosion(Camera& camera) {
	// Add a camera/controller shake and flash for when the rocket explodes...
	camera.SetCameraShake(1.2f, Vector3D(0.9f, 0.8f, 0.1f), 130);
	GameControllerManager::GetInstance()->VibrateControllers(1.2f, BBBGameController::VeryHeavyVibration, BBBGameController::VeryHeavyVibration);

	this->flashHUD->Activate(0.5, 1.0f);
	// Play the explosion sound
	this->soundAssets->PlayWorldSound(GameSoundAssets::WorldSoundRocketExplodedEvent, GameSoundAssets::VeryLoudVolume);
}

/*
 * This will load a set of assets for use in a game based off a
 * given world-style, after loading all assets will be available for use in-game.
 * Precondition: world != NULL.
 */
void GameAssets::LoadWorldAssets(const GameWorld& world) {
	// Delete the currently loaded world and level assets if there are any
	// Delete all the levels for the world that are currently loaded - THIS MUST BE CALLED BEFORE DELETING
	// THE WORLD ASSETS!!!
	if (this->currentLevelMesh != NULL) {
		delete this->currentLevelMesh;
		this->currentLevelMesh = NULL;
	}

	// Check to see if we've already loaded the world assets...
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading world assets...");
	if (this->worldAssets == NULL || this->worldAssets->GetStyle() != world.GetStyle()) {
		// Delete all previously loaded style-related assets
		delete this->worldAssets;
		this->worldAssets = NULL;

		// Load up the new set of world geometry assets
		this->worldAssets = GameWorldAssets::CreateWorldAssets(world.GetStyle());
		assert(this->worldAssets != NULL);

		// Unload any previous world's sound assets and load the new world sound assets
		this->soundAssets->SetActiveWorldSounds(world.GetStyle(), true, true);
	}
	else {
		// Make sure the sound and world assets are in sync
		assert(this->soundAssets->GetActiveWorldSounds() == world.GetStyle());
	}
	this->worldAssets->ResetToInitialState();

	LoadingScreen::GetInstance()->UpdateLoadingScreen(LoadingScreen::ABSURD_LOADING_DESCRIPTION);

	// Reinitialize the life HUD elements
	this->lifeHUD->Reinitialize();
}

void GameAssets::ActivateRandomItemEffects(const GameModel& gameModel, const GameItem& actualItem) {
	// Figure out the item texture associated with the actual item...
	Texture2D* actualItemTexture = this->itemAssets->GetItemTexture(actualItem.GetItemType());
	// This will start and animation that shows to the player what the random item has become
	this->randomToItemAnimation.Start(actualItemTexture, gameModel);
}

/**
 * Activate the effect for a particular item - this can be anything from changing lighting to
 * making pretty eye-candy-particles.
 */
void GameAssets::ActivateItemEffects(const GameModel& gameModel, const GameItem& item) {
	// First deal with any particle related effects
	this->espAssets->SetItemEffect(item, gameModel);
	// Also make the FBO assets aware of a newly active effect
	this->fboAssets->ActivateItemEffects(item);

	// Deal with any light changes...
	switch (item.GetItemType()) {
		case GameItem::BlackoutItem: {
				// Turn the lights off and make only the paddle and ball visible.
				assert(gameModel.IsBlackoutEffectActive());
				this->lightAssets->ToggleLights(false);
			}
			break;
			
		case GameItem::PoisonPaddleItem: {
				// The poison item will make the lights turn a sickly green colour
				assert((gameModel.GetPlayerPaddle()->GetPaddleType() & PlayerPaddle::PoisonPaddle) == PlayerPaddle::PoisonPaddle);

				this->lightAssets->StartStrobeLight(GameLightAssets::FGKeyLight, GameViewConstants::GetInstance()->POISON_LIGHT_LIGHT_COLOUR, 1.0f);
				this->lightAssets->StartStrobeLight(GameLightAssets::FGFillLight, GameViewConstants::GetInstance()->POISON_LIGHT_DEEP_COLOUR, 1.0f);
				this->lightAssets->StartStrobeLight(GameLightAssets::BallKeyLight, GameViewConstants::GetInstance()->POISON_LIGHT_LIGHT_COLOUR, 1.0f);
			}
		  break;

		case GameItem::PaddleCamItem: {
				// For the paddle camera we remove the stars from all currently falling items (block the view of the player)
				// NOTE that this is not permanent and just does so for any currently falling items
				this->espAssets->TurnOffCurrentItemDropStars();
				
				// We make the safety net transparent so that it doesn't obstruct the paddle cam... too much
				assert(this->GetCurrentLevelMesh() != NULL);
				this->GetCurrentLevelMesh()->PaddleCameraActiveToggle(true);

				// Move the key light in the foreground so that it is behind the camera when it goes into
				// paddle cam mode.
				float halfLevelHeight = gameModel.GetCurrentLevel()->GetLevelUnitHeight() / 2.0f;
				float halfLevelWidth  = gameModel.GetCurrentLevel()->GetLevelUnitWidth() / 2.0f;

				Point3D newFGKeyLightPos(halfLevelWidth, -(halfLevelHeight + 20.0f), 0.0f);
				Point3D newFGFillLightPos(-halfLevelWidth, -(halfLevelHeight + 20.0f), 0.0f);
				
				this->lightAssets->ChangeLightPosition(GameLightAssets::FGKeyLight, newFGKeyLightPos, 2.0f);
				this->lightAssets->ChangeLightPosition(GameLightAssets::FGFillLight, newFGFillLightPos, 2.0f);

				// Fade out the background...
				this->worldAssets->FadeBackground(true, 2.0f);
			}
			break;

		case GameItem::BallCamItem: {
				// For the paddle camera we remove the stars from all currently falling items (block the view of the player)
				// NOTE that this is not permanent and just does so for any currently falling items
				this->espAssets->TurnOffCurrentItemDropStars();

				// Change the position of the key light so that it is facing down with the ball
				float halfLevelHeight = gameModel.GetCurrentLevel()->GetLevelUnitHeight() / 2.0f;

				Point3D newFGKeyLightPos(0.0f, (halfLevelHeight + 10.0f), 0.0f);
				Point3D newFGFillLightPos(0.0f, -(halfLevelHeight + 10.0f), 0.0f);
				
				this->lightAssets->ChangeLightPosition(GameLightAssets::FGKeyLight, newFGKeyLightPos, 2.0f);
				this->lightAssets->ChangeLightPosition(GameLightAssets::FGFillLight, newFGFillLightPos, 2.0f);

				// Fade out the background...
				this->worldAssets->FadeBackground(true, 2.0f);
			}
			break;

		case GameItem::ShieldPaddleItem: {
				const Texture2D* fullscreenFBOTex = this->GetFBOAssets()->GetFinalFullScreenFBO()->GetFBOTexture();
				assert(fullscreenFBOTex != NULL);
				this->paddleShield->ActivateShield(*gameModel.GetPlayerPaddle(), *fullscreenFBOTex);
			}
			break;

		default:
			break;
	}
}

/**
 * Deactivate the effect for a particular item - this will clean up anything that was activated
 * which needs manual clean up.
 */
void GameAssets::DeactivateItemEffects(const GameModel& gameModel, const GameItem& item) {
	// Also make the FBO assets aware of the deactivated effect
	this->fboAssets->DeactivateItemEffects(item);

	// Deal with any light changes...
	switch (item.GetItemType()) {
		case GameItem::BlackoutItem: {
				// Turn the lights back on and revert lights back to their defaults
				assert(!gameModel.IsBlackoutEffectActive());
				this->lightAssets->ToggleLights(true);
			}
		  break;

		case GameItem::PoisonPaddleItem: {
				// Stop strobing the lights
				this->lightAssets->StopStrobeLight(GameLightAssets::FGKeyLight);
				this->lightAssets->StopStrobeLight(GameLightAssets::FGFillLight);
				this->lightAssets->StopStrobeLight(GameLightAssets::BallKeyLight);
			}
		  break;

		case GameItem::PaddleCamItem: {
				// We make the safety net visible (if activated) again
				assert(this->GetCurrentLevelMesh() != NULL);
				this->GetCurrentLevelMesh()->PaddleCameraActiveToggle(false);

				// Move the foreground key and fill lights back to their default positions...
				this->lightAssets->RestoreLightPosition(GameLightAssets::FGKeyLight, 2.0f);
				this->lightAssets->RestoreLightPosition(GameLightAssets::FGFillLight, 2.0f);

				// Show the background once again...
				this->worldAssets->FadeBackground(false, 2.0f);
			}
			break;

		case GameItem::BallCamItem: 
			// Move the foreground key and fill lights back to their default positions...
			this->lightAssets->RestoreLightPosition(GameLightAssets::FGKeyLight, 2.0f);
			this->lightAssets->RestoreLightPosition(GameLightAssets::FGFillLight, 2.0f);

			// Show the background once again...
			this->worldAssets->FadeBackground(false, 2.0f);
			break;

		case GameItem::ShieldPaddleItem: {
				this->paddleShield->DeactivateShield();
			}
			break;

		default:
			break;
	}
}

/**
 * Deactivate all misc. effects in the view that may not be deactivated from items alone.
 */
void GameAssets::DeactivateMiscEffects() {
	this->fboAssets->DeactivateInkSplatterEffect();
	this->espAssets->KillAllActiveEffects();
	this->espAssets->KillAllActiveTeslaLightningArcs();
	this->itemAssets->ClearTimers();
	// Disable any overlay stuff
	this->painHUD->Deactivate();
	this->randomToItemAnimation.Stop();
}

/**
 * Activate effects associated with the final ball about to die - this should
 * be called just as the last ball is spiralling towards its death.
 */
void GameAssets::ActivateLastBallDeathEffects(const GameBall& lastBall) {
	UNUSED_PARAMETER(lastBall);

	// Fade out the world background since the camera is going to be moving places we don't
	// want the player to see
	this->worldAssets->FadeBackground(true, 0.2f);
}

/**
 * Deactivate any effects that need to be deactivated from when the last
 * ball was dieing.
 */
void GameAssets::DeactivateLastBallDeathEffects() {
	// Bring the world background back into view
	this->worldAssets->FadeBackground(false, 2.0f);
}