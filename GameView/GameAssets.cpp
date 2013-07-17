/**
 * GameAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
#include "PointsHUD.h"
#include "BallBoostHUD.h"
#include "BallReleaseHUD.h"
#include "RemoteControlRocketHUD.h"
#include "StickyPaddleGoo.h"
#include "LaserPaddleGun.h"
#include "RocketMesh.h"
#include "PaddleShield.h"
#include "OmniLaserBallEffect.h"
#include "MagnetPaddleEffect.h"
#include "MineMeshManager.h"

// Game Model includes
#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/Beam.h"
#include "../GameModel/PaddleRocketProjectile.h"
#include "../GameModel/FireGlobProjectile.h"
#include "../GameModel/PaddleMineProjectile.h"
#include "../GameModel/PaddleRemoteControlRocketProjectile.h"
#include "../GameModel/FullscreenFlashEffectInfo.h"

// Game Sound Includes
#include "../GameSound/GameSound.h"

// Game Control Includes
#include "../GameControl/GameControllerManager.h"

// Blammo Engine includes
#include "../BlammoEngine/Texture3D.h"

// ESP Engine includes
#include "../ESPEngine/ESPEmitter.h"

// Basic Top Level Includes
#include "../ResourceManager.h"

// *****************************************************

GameAssets::GameAssets(int screenWidth, int screenHeight, GameSound* sound): 
sound(sound),

currentLevelMesh(NULL),
worldAssets(NULL),
espAssets(NULL),
itemAssets(NULL),
fboAssets(NULL),
lightAssets(NULL),
tutorialAssets(NULL),

lifeHUD(NULL),
crosshairHUD(NULL),
painHUD(NULL),
flashHUD(NULL),
pointsHUD(NULL),
boostHUD(NULL),
ballReleaseHUD(NULL),
remoteControlRocketHUD(NULL),

ball(NULL), 
spikeyBall(NULL),
rocketMesh(NULL),
mineMeshMgr(NULL),
paddleBeamAttachment(NULL),
paddleMineAttachment(NULL),
paddleLaserAttachment(NULL),
paddleStickyAttachment(NULL),
paddleShield(NULL),
ballSafetyNet(NULL),

invisibleEffect(NULL), 
ghostBallEffect(NULL),
fireBallEffect(NULL),

omniLaserBallEffect(NULL),
magnetPaddleEffect(NULL)
{
	// Load ESP assets
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading purdy pictures...");
	this->espAssets      = new GameESPAssets();
    this->tutorialAssets = new GameTutorialAssets();

	// Load FBO assets
    this->fboAssets = new GameFBOAssets(screenWidth, screenHeight, this->sound);

	// Load item assets
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading game items...");
	this->itemAssets = new GameItemAssets(this->espAssets);
	bool didItemAssetsLoad = this->itemAssets->LoadItemAssets();
    UNUSED_VARIABLE(didItemAssetsLoad);
	assert(didItemAssetsLoad);

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
	this->crosshairHUD	 = new CrosshairLaserHUD();
	this->lifeHUD        = new LivesLeftHUD();
	this->painHUD        = new PlayerHurtHUD();
	this->flashHUD       = new FlashHUD();
    this->pointsHUD      = new PointsHUD();
    this->boostHUD       = new BallBoostHUD(screenHeight);
    this->ballReleaseHUD = new BallReleaseHUD();
    this->remoteControlRocketHUD = new RemoteControlRocketHUD(*this);

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

	bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->paddleBeamAttachment);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->paddleMineAttachment);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->ball);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->spikeyBall);
    assert(success);

    UNUSED_VARIABLE(success);

	delete this->rocketMesh;
	this->rocketMesh = NULL;
    delete this->mineMeshMgr;
    this->mineMeshMgr = NULL;
	delete this->paddleLaserAttachment;
	this->paddleLaserAttachment = NULL;
	delete this->paddleStickyAttachment;
	this->paddleStickyAttachment = NULL;
	delete this->paddleShield;
	this->paddleShield = NULL;

    delete this->ballSafetyNet;
    this->ballSafetyNet = NULL;

    // Clean up tutotial assets
    delete this->tutorialAssets;
    this->tutorialAssets = NULL;

	// Clear up the FrameBuffer Object assets
	delete this->fboAssets;
	this->fboAssets = NULL;

	// Clear up the light assets
	delete this->lightAssets;
	this->lightAssets = NULL;

	// Delete any HUD objects
	delete this->lifeHUD;
	this->lifeHUD = NULL;
	delete this->crosshairHUD;
	this->crosshairHUD = NULL;
	delete this->painHUD;
	this->painHUD = NULL;
	delete this->flashHUD;
	this->flashHUD = NULL;
    delete this->pointsHUD;
    this->pointsHUD = NULL;
    delete this->boostHUD;
    this->boostHUD = NULL;
    delete this->ballReleaseHUD;
    this->ballReleaseHUD = NULL;
    delete this->remoteControlRocketHUD;
    this->remoteControlRocketHUD = NULL;
}

/**
 * Called before drawing the balls - used to add effects that need to be behind the balls.
 */
void GameAssets::DrawGameBallsPreEffects(double dT, GameModel& gameModel, const Camera& camera) {
    UNUSED_PARAMETER(dT);

    // Exit immediately if there's no bullet time effects currently active
    if (gameModel.GetBallBoostModel() == NULL || !gameModel.GetBallBoostModel()->IsInBulletTime()) {
        return;
    }

    // Ball boost direction (should already be normalized)
    Vector2D ballBoostDir = gameModel.GetBallBoostModel()->GetBallBoostDirection();
    Vector2D ballBoostRotated90(-ballBoostDir[1], ballBoostDir[0]);
    ballBoostDir *= 2*LevelPiece::PIECE_WIDTH;

    // Calculate an alpha based on how far into the bullet time the player
    // currently is and always have a bit of alpha so the player can always
    // see the boost streaks behind the ball(s)
    float alpha = 0.15f + (0.5f * static_cast<float>(gameModel.GetBallBoostModel()->GetTotalBulletTimeElapsed() / 
                                                     BallBoostModel::GetMaxBulletTimeDuration()));

    // Generate a screen aligned matrix from the current camera
    static Matrix4x4 screenAlignedMatrix;
    screenAlignedMatrix = camera.GenerateScreenAlignMatrix();

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Go through each ball in the game, draw any pre-effects accordingly
	const std::list<GameBall*>& balls = gameModel.GetGameBalls();
	for (std::list<GameBall*>::const_iterator ballIter = balls.begin(); ballIter != balls.end(); ++ballIter) {
        GameBall* currBall = *ballIter;
        if (!currBall->IsBallAllowedToBoost()) {
            continue;
        }
        
        Vector2D fullRadiusVec = 0.99f * currBall->GetBounds().Radius() * ballBoostRotated90;
        Vector2D halfRadiusVec = 0.5f  * fullRadiusVec;

        const Point2D& ballPos = currBall->GetCenterPosition2D();
        glPushMatrix();
        glTranslatef(ballPos[0], ballPos[1], 0);
        glMultMatrixf(screenAlignedMatrix.begin());
        
        // Draw the movement streaks behind the ball in the direction 
        // away from where the boost will take place
        glBegin(GL_QUADS);

        // First quad is biggest and extends behind the ball
        glColor4f(1, 1, 1, alpha);
        glVertex3f(fullRadiusVec[0], fullRadiusVec[1], 0);
        glVertex3f(-fullRadiusVec[0], -fullRadiusVec[1], 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(-fullRadiusVec[0] - ballBoostDir[0], -fullRadiusVec[1] - ballBoostDir[1], 0);
        glVertex3f(fullRadiusVec[0] - ballBoostDir[0], fullRadiusVec[1] - ballBoostDir[1], 0);

        // Next quad is a bit smaller and fits inside
        glColor4f(1, 1, 1, alpha);
        glVertex3f(halfRadiusVec[0], halfRadiusVec[1], 0);
        glVertex3f(-halfRadiusVec[0], -halfRadiusVec[1], 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(-halfRadiusVec[0] - ballBoostDir[0], -halfRadiusVec[1] - ballBoostDir[1], 0);
        glVertex3f(halfRadiusVec[0] - ballBoostDir[0], halfRadiusVec[1] - ballBoostDir[1], 0);

        glEnd();

        glPopMatrix();
    }

    glPopAttrib();
}

void GameAssets::DrawGameBallsBoostPostEffects(double dT, GameModel& gameModel, const Camera& camera) {
    // This will draw any affects that occur right after a boost takes place
    this->espAssets->DrawBallBoostingEffects(dT, camera);

    // Exit immediately if there's no bullet time effects currently active
    if (gameModel.GetBallBoostModel() == NULL || !gameModel.GetBallBoostModel()->IsInBulletTime()) {
        return;
    }

    // This will draw any effects during the ball boost bullet-time
    this->espAssets->DrawBulletTimeBallsBoostEffects(dT, camera, gameModel);
}

// Draw the game's ball(s) (the thing that bounces and blows stuff up), position it, 
// draw the materials and draw the mesh.
void GameAssets::DrawGameBalls(double dT, GameModel& gameModel, const Camera& camera, const Vector2D& worldT) {
	this->DrawGameBallsPreEffects(dT, gameModel, camera);

	// Average values used to calculate the colour and position of the ball light
	Point3D avgBallPosition(0,0,0);
	Colour avgBallColour(0,0,0);
	unsigned int visibleBallCount = 0;
    bool ballWithOmniLaserDrawn = false;

	// Go through each ball in the game, draw it accordingly
	const std::list<GameBall*>& balls = gameModel.GetGameBalls();
	for (std::list<GameBall*>::const_iterator ballIter = balls.begin(); ballIter != balls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		
		bool ballIsInvisible = (currBall->GetBallType() & GameBall::InvisiBall) == GameBall::InvisiBall;
		CgFxEffectBase* ballEffectTemp = NULL;
		Colour currBallColour(0,0,0);

		const Point2D& ballPos = currBall->GetCenterPosition2D();

		if (currBall->GetBallType() == GameBall::NormalBall) {
			// Normal ball with a regular light
			avgBallPosition = avgBallPosition + Vector3D(ballPos[0], ballPos[1], 0);
			avgBallColour = avgBallColour + GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_COLOUR;
			visibleBallCount++;
		}
		else {
			// The ball has an item in effect on it... figure out what effect(s) and render it/them appropriately

			unsigned int numColoursApplied = 0;
			
            if (!ballIsInvisible) {

                // SLOW AND FAST BALL CHECK
                if ((currBall->GetBallType() & GameBall::SlowBall) == GameBall::SlowBall) {
				    // We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
                        this->espAssets->DrawSlowBallEffects(dT, camera, *currBall);
				    }
                }
                else if ((currBall->GetBallType() & GameBall::FastBall) == GameBall::FastBall) {
				    // We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
                        this->espAssets->DrawFastBallEffects(dT, camera, *currBall);
				    }
                }

			    // GHOST BALL CHECK
			    if ((currBall->GetBallType() & GameBall::GhostBall) == GameBall::GhostBall) {
    				
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
			    if ((currBall->GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
    				
				    // Draw when uber ball and not invisiball...
				    // We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
					    this->espAssets->DrawUberBallEffects(dT, camera, *currBall);
				    }

				    currBallColour = currBallColour + GameModelConstants::GetInstance()->UBER_BALL_COLOUR;
				    numColoursApplied++;
			    }

			    // GRAVITY BALL CHECK
			    if ((currBall->GetBallType() & GameBall::GraviBall) == GameBall::GraviBall) {

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
			    if ((currBall->GetBallType() & GameBall::CrazyBall) == GameBall::CrazyBall) {

				    // Draw when crazy ball when not invisible...
				    // We don't draw any of the effects if we're in ball camera mode
				    if (!GameBall::GetIsBallCameraOn()) {				
					    this->espAssets->DrawCrazyBallEffects(dT, camera, *currBall);
				    }
    				
				    currBallColour = currBallColour + GameModelConstants::GetInstance()->CRAZY_BALL_COLOUR;
				    numColoursApplied++;
			    }

			    // FIRE BALL CHECK
			    if ((currBall->GetBallType() & GameBall::FireBall) == GameBall::FireBall) {
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
			    if ((currBall->GetBallType() & GameBall::IceBall) == GameBall::IceBall) {
				    //ballEffectTemp = this->iceBallEffect;

				    // Draw the ice ball when not invisible
				    // We don't draw any of the effects if we're in a ball camera mode or inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
					    this->espAssets->DrawIceBallEffects(dT, camera, *currBall);
				    }
				    currBallColour = currBallColour + GameModelConstants::GetInstance()->ICE_BALL_COLOUR;
				    numColoursApplied++;
			    }

                // OMNI LASER BALL CHECK
                if ((currBall->GetBallType() & GameBall::OmniLaserBulletBall) == GameBall::OmniLaserBulletBall) {
                    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
					    this->omniLaserBallEffect->Draw(dT, !ballWithOmniLaserDrawn, camera, *currBall);
                        ballWithOmniLaserDrawn = true;
				    }
                    currBallColour = currBallColour + GameModelConstants::GetInstance()->OMNI_LASER_BALL_COLOUR;
                    numColoursApplied++;
                }

				// We only take the average of visible balls.
				avgBallPosition = avgBallPosition + Vector3D(ballPos[0], ballPos[1], 0);
			
				numColoursApplied = std::max<int>(1, numColoursApplied);
				avgBallColour = avgBallColour + (currBallColour / numColoursApplied);

				visibleBallCount++;
            }
            else {
                // ballIsInvisible == true

                // INVISIBALL:
                // Obtain the POST full screen rendering from the previous frame - this is
				// a bit of a hack but it saves us from reading/writing to the same FBO simultaneously
                this->invisibleEffect->SetFBOTexture(this->fboAssets->GetFullSceneFBO()->GetFBOTexture());
				ballEffectTemp = this->invisibleEffect;
            }
		}

        // Obtain the colour of the ball from the model, if it's completely invisible
        // then just skip all of the draw calls
        const ColourRGBA& ballColour = currBall->GetColour();
        if (ballColour.A() <= 0.0f) {
            continue;
        }

		// Draw the ball model...
		glPushMatrix();
		float ballScaleFactor = currBall->GetBallScaleFactor();
		glTranslatef(ballPos[0], ballPos[1], 0);
		
		// Draw background/pre-effects for the ball
		this->espAssets->DrawBackgroundBallEffects(dT, camera, *currBall);

		Vector3D ballRot = currBall->GetRotation();
		glRotatef(ballRot[0], 1.0f, 0.0f, 0.0f);
		glRotatef(ballRot[1], 0.0f, 1.0f, 0.0f);
		glRotatef(ballRot[2], 0.0f, 0.0f, 1.0f);
		glScalef(ballScaleFactor, ballScaleFactor, ballScaleFactor);

		BasicPointLight ballKeyLight, ballFillLight;
		this->lightAssets->GetBallAffectingLights(ballKeyLight, ballFillLight);

		// Always make the colour of the invisiball to be plain white - we don't want to affect
		// the cloaking effect
        glPushAttrib(GL_DEPTH_BUFFER_BIT);
		if (ballIsInvisible) {
			glColor4f(1.0f, 1.0f, 1.0f, ballColour.A());

            // Disable the depth draw, this will make sure the ball doesn't get outlined
            glDepthMask(GL_FALSE);
		}
		else {
			glColor4f(ballColour.R(), ballColour.G(), ballColour.B(), ballColour.A());
		}

		if ((currBall->GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
			this->spikeyBall->Draw(camera, ballEffectTemp, ballKeyLight, ballFillLight);
		}
		else {
			this->ball->Draw(camera, ballEffectTemp, ballKeyLight, ballFillLight);
		}
        glPopAttrib();

		glPopMatrix();
	}

    // If the ball boost direction is being decided then we need to draw the boost effect
    this->DrawGameBallsBoostPostEffects(dT, gameModel, camera);

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

    glPushAttrib(GL_DEPTH_BUFFER_BIT);
    if (p.HasPaddleType(PlayerPaddle::InvisiPaddle) || p.GetAlpha() < 1.0f) {
        // Disable the depth draw, this will make sure the paddle doesn't get outlined
        glDepthMask(GL_FALSE);
    }

    // If the paddle is invisible then we don't draw any of the extraneous effects on it and we make sure
    // to draw it with an invisible material...
    CgFxEffectBase* paddleReplacementMat = NULL;
    if (p.HasPaddleType(PlayerPaddle::InvisiPaddle)) {
        paddleReplacementMat = this->invisibleEffect;
        // Just tick the effects on the paddle, no drawing
        this->espAssets->TickButDontDrawBackgroundPaddleEffects(dT);
    }
    else {
	    // Draw any effects on the paddle (e.g., item acquiring effects)
	    this->espAssets->DrawBackgroundPaddleEffects(dT, camera, p);
    }

    if (!p.GetIsPaddleCameraOn() && p.HasPaddleType(PlayerPaddle::MagnetPaddle)) {
        this->magnetPaddleEffect->Draw(dT, p);
    }

	// Draw the paddle
	this->worldAssets->DrawPaddle(p, camera, paddleReplacementMat, paddleKeyLight, paddleFillLight, ballLight);

    glPopAttrib();

	if (p.HasPaddleType(PlayerPaddle::LaserBeamPaddle)) {
		if (p.GetIsLaserBeamFiring()) {
			// Draw paddle blast-y stuff when firing
			if (!p.GetIsPaddleCameraOn()) {
				this->espAssets->DrawPaddleLaserBeamFiringEffects(dT, camera, p);
			}
		}
		// Beam isn't firing yet, if the rocket is on the paddle then don't draw any of the beam effects
		// coming out of the paddle
		else if (!p.HasPaddleType(PlayerPaddle::RocketPaddle)){
			// Draw glow-y beam origin when beam is able to fire but not actually firing yet
			this->espAssets->DrawPaddleLaserBeamBeforeFiringEffects(dT, camera, p);
		}
	}

    /*
    // TODO:
    if (p.HasPaddleType(PlayerPaddle::MineLauncherPaddle)) {
        // Draw an effect on the paddle to indicate that there is a mine launcher attachment activated,
        // but we only draw it if there isn't a rocket on the paddle
		if (!p.HasPaddleType(PlayerPaddle::RocketPaddle)) {
			this->espAssets->DrawPaddleMineActiveEffects(dT, camera, p);
		}
    }
    */

	// In the case of a laser bullet paddle (and NOT paddle camera mode), we draw the laser attachment and its related effects
	// Camera mode is exempt from this because the attachment would seriously get in the view of the player
    if (!p.GetIsPaddleCameraOn() && p.GetAlpha() > 0.0f &&
         p.HasPaddleType(PlayerPaddle::LaserBulletPaddle | PlayerPaddle::LaserBeamPaddle | PlayerPaddle::MineLauncherPaddle)) {

		glColor4f(1.0f, 1.0f, 1.0f, p.GetAlpha());

        glPushAttrib(GL_DEPTH_BUFFER_BIT);
        if (p.HasPaddleType(PlayerPaddle::InvisiPaddle) || p.GetAlpha() < 1.0f) {
            // Disable the depth draw, this will make sure the paddle attachments don't get outlined
            glDepthMask(GL_FALSE);
        }

        // Draw the various active attachments...
        if (p.HasPaddleType(PlayerPaddle::LaserBulletPaddle)) {
			this->paddleLaserAttachment->Draw(dT, p, camera, paddleReplacementMat, paddleKeyLight, paddleFillLight, ballLight);
		}

        glPushMatrix();
        glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
        glRotatef(p.GetZRotation(), 0, 0, 1);
       
		if (p.HasPaddleType(PlayerPaddle::LaserBeamPaddle)) {
			this->paddleBeamAttachment->Draw(camera, paddleReplacementMat, paddleKeyLight, paddleFillLight, ballLight);
		}
        if (p.HasPaddleType(PlayerPaddle::MineLauncherPaddle)) {
            this->mineMeshMgr->DrawLoadingMine(dT, p, camera, paddleKeyLight, paddleFillLight, ballLight);
			this->paddleMineAttachment->Draw(camera, paddleReplacementMat, paddleKeyLight, paddleFillLight, ballLight);
		}

        glPopMatrix();
        glPopAttrib();
	}

	glPopMatrix();
	debug_opengl_state();
}

/**
 * Used to draw any relevant post-processing-related effects for the paddle.
 */
void GameAssets::DrawPaddlePostEffects(double dT, GameModel& gameModel, const Camera& camera, FBObj* sceneFBO) {
	PlayerPaddle* paddle = gameModel.GetPlayerPaddle();

	// Do nothing if we are in paddle camera mode
	if (paddle->GetIsPaddleCameraOn() || paddle->GetAlpha() <= 0.0f) {
		return;
	}
	
	const Point2D& paddleCenter = paddle->GetCenterPosition();
	float paddleScaleFactor = paddle->GetPaddleScaleFactor();
	float scaleHeightAdjustment = PlayerPaddle::PADDLE_HALF_HEIGHT * (paddleScaleFactor - 1);

	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	glTranslatef(paddleCenter[0], paddleCenter[1] + scaleHeightAdjustment, 0);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	

	if (!paddle->GetIsPaddleCameraOn()) {
		// Draw the shield around the paddle (if it's currently activated)
		this->paddleShield->DrawAndTick(*paddle, camera, dT);
	}

	// When the paddle has the 'sticky' power-up (and doesnt have a shield on) we attach sticky goo to its top
	if (paddle->HasPaddleType(PlayerPaddle::StickyPaddle) && !paddle->HasPaddleType(PlayerPaddle::ShieldPaddle)) {

		// Set the texture for the refraction in the goo - be careful here, we can't get
		// the 'post full scene' fbo because we are currently in the middle of drawing to it
		this->paddleStickyAttachment->SetSceneTexture(sceneFBO->GetFBOTexture());

		// In the case where the paddle laser beam is also active then the beam will refract through the goo,
		// illuminate the goo a lot more
		if (paddle->HasPaddleType(PlayerPaddle::LaserBeamPaddle) && paddle->GetIsLaserBeamFiring()) {
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
	if (paddle->HasPaddleType(PlayerPaddle::LaserBulletPaddle) && !paddle->HasPaddleType(PlayerPaddle::RocketPaddle)) {
		// Draw glow-y effects where the laser originates...
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

void GameAssets::FastDrawBackgroundModel() {
    this->worldAssets->FastDrawBackgroundModel();
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

void GameAssets::DrawBoss(double dT, const GameLevel* currLevel, const Camera& camera) {

    if (currLevel->GetHasBoss()) {
        Vector3D worldTransform(-currLevel->GetLevelUnitWidth()/2.0f, -currLevel->GetLevelUnitHeight()/2.0f, 0.0f);
        
        glPushMatrix();
	    glTranslatef(worldTransform[0], worldTransform[1], worldTransform[2]);

	    BasicPointLight fgKeyLight, fgFillLight, ballLight;
	    this->lightAssets->GetBossAffectingLights(fgKeyLight, fgFillLight, ballLight);
        this->GetCurrentLevelMesh()->DrawBoss(dT, camera, fgKeyLight, fgFillLight, ballLight, 
            this->fboAssets->GetFullSceneFBO()->GetFBOTexture());

        glPopMatrix();
    }
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

    static const float BRIGHT_BEAM_CENTER_MULTIPLER = 2.28f;
	static const float TYPICAL_BEAM_ALPHA = 0.4f;
	const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
	float quarterPaddleDepth = paddle->GetHalfDepthTotal() / 2.0f;

    
	glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glTranslatef(0, 0, 0);

	glBegin(GL_QUADS);

	Point3D temp;
	Point3D beamSegStart, beamSegEnd;
	Vector3D beamRightVec;
	Vector3D beamUpVec;
	Vector3D beamDepthVec;
	float currRadius, currAlpha;
	Vector2D tempRadiusOffset;
	
    bool paddleLaserBeamActive = false;

	for (std::list<Beam*>::const_iterator beamIter = beams.begin(); beamIter != beams.end(); ++beamIter) {
		const Beam* currentBeam = *beamIter;
        currAlpha = currentBeam->GetBeamAlpha();

        // Special cases for paddle laser beams -- if the paddle has been removed from the game then
        // we don't want to draw its beams either
        if (currentBeam->GetType() == Beam::PaddleBeam) {
            if (paddle->HasBeenPausedAndRemovedFromGame(gameModel.GetPauseState())) {
                continue;
            }
            else {
                paddleLaserBeamActive = true;
            }
        }
        
        const Colour& beamColour = currentBeam->GetBeamColour();
        Colour beamCenterColour  = BRIGHT_BEAM_CENTER_MULTIPLER * beamColour;
        
		const std::list<BeamSegment*>& beamSegments = currentBeam->GetBeamParts();
		std::list<BeamSegment*>::const_iterator segmentIter = beamSegments.begin();
		
		int segCounter = 0;
		const int NUM_BASE_SEGMENTS = currentBeam->GetNumBaseBeamSegments();

		for (; segmentIter != beamSegments.end(); ++segmentIter, ++segCounter) {

            // Don't show the first few segments of the beam if it's a paddle beam and the
            // paddle camera item is active, otherwise it will fill the screen with beaminess
            // and just be annoying
            if (currentBeam->GetType() == Beam::PaddleBeam && 
                paddle->GetIsPaddleCameraOn() && segCounter < NUM_BASE_SEGMENTS) {
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

			glColor4f(beamCenterColour.R(), beamCenterColour.G(), beamCenterColour.B(), TYPICAL_BEAM_ALPHA*currAlpha);
			
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

			glColor4f(beamColour.R(), beamColour.G(), beamColour.B(), TYPICAL_BEAM_ALPHA*currAlpha);

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

    // If in paddle cam mode draw a fullscreen quad with the laser colour
    if (paddle->GetIsPaddleCameraOn() && paddleLaserBeamActive) {
        GeometryMaker::GetInstance()->DrawFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 0.0, 
            ColourRGBA(GameModelConstants::GetInstance()->PADDLE_LASER_BEAM_COLOUR, 0.2f));
    }

	debug_opengl_state();
}

void GameAssets::DrawMeshProjectiles(double dT, const GameModel& gameModel, const Camera& camera) {
    // Get lights affecting the foreground meshes...
    BasicPointLight keyLight, fillLight, ballLight;
    this->lightAssets->GetPieceAffectingLights(keyLight, fillLight, ballLight);

    // Draw any rockets that are currently active...
    this->rocketMesh->Draw(dT, *gameModel.GetPlayerPaddle(), camera, keyLight, fillLight, ballLight);

    // Draw any mines that are currently active...
    this->mineMeshMgr->Draw(dT, camera, keyLight, fillLight, ballLight);
}

/**
 * Draw informational game elements (e.g., tutorial bubbles, information about items acquired, etc.).
 */
void GameAssets::DrawInformativeGameElements(const Camera& camera, double dT, const GameModel& gameModel) {
	if (!this->randomToItemAnimation.GetIsActive()) {
		return;
	}

    // Don't show and stop the informative random item animation if the player is in ball camera or
    // paddle camera mode
	const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
    if (GameBall::GetIsBallCameraOn() || paddle->GetIsPaddleCameraOn()) {
        this->randomToItemAnimation.Stop();
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

const Point2D& GameAssets::GetBallSafetyNetPosition() const {
    return this->ballSafetyNet->GetSafetyNetCenterPosition();
}

/**
 * Call when the ball safety net has been created. This will prompt any animations /
 * effects associated with the mesh representing the safety net.
 */
void GameAssets::BallSafetyNetCreated() {
	this->ballSafetyNet->CreateBallSafetyNet();
}

/**
 * Call when the ball safety net has been destroyed this will prompt any
 * animations / effects associated with the mesh representing the safety net.
 */
void GameAssets::BallSafetyNetDestroyed(const GameLevel& currLevel, const Point2D& pos) {
	this->ballSafetyNet->DestroyBallSafetyNet(currLevel, pos[0]);
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
    
    // Setup the ball safety net for the new level...
	this->ballSafetyNet->Regenerate(currLevel);
}

void GameAssets::LoadRegularMeshAssets() {
	if (this->ball == NULL) {
		this->ball = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BALL_MESH);
	}
	if (this->spikeyBall == NULL) {
		this->spikeyBall = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SPIKEY_BALL_MESH);
	}
	if (this->rocketMesh == NULL) {
		this->rocketMesh = new RocketMesh();
	}
    if (this->mineMeshMgr == NULL) {
        this->mineMeshMgr = new MineMeshManager();
    }
	if (this->paddleBeamAttachment == NULL) {
		this->paddleBeamAttachment = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_BEAM_ATTACHMENT_MESH);
	}
    if (this->paddleMineAttachment == NULL) {
        this->paddleMineAttachment = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_MINE_ATTACHMENT_MESH);
    }
	if (this->paddleLaserAttachment == NULL) {
		this->paddleLaserAttachment = new LaserPaddleGun();
	}
	if (this->paddleStickyAttachment == NULL) {
		this->paddleStickyAttachment = new StickyPaddleGoo();
	}
    if (this->ballSafetyNet == NULL) {
        this->ballSafetyNet = new BallSafetyNetMesh();
    }
}

void GameAssets::LoadRegularEffectAssets() {
	if (this->invisibleEffect == NULL) {
		this->invisibleEffect = new CgFxPostRefract();
		this->invisibleEffect->SetWarpAmountParam(50.0f);
		this->invisibleEffect->SetIndexOfRefraction(1.33f);
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
    if (this->omniLaserBallEffect == NULL) {
        this->omniLaserBallEffect = new OmniLaserBallEffect();
    }
    if (this->magnetPaddleEffect == NULL) {
        this->magnetPaddleEffect = new MagnetPaddleEffect();
    }
	if (this->paddleShield == NULL) {
		this->paddleShield = new PaddleShield();
	}
}

void GameAssets::DeleteRegularEffectAssets() {
	// Delete the invisiball effect
	if (this->invisibleEffect != NULL) {
		delete this->invisibleEffect;
		this->invisibleEffect = NULL;
	}

	if (this->ghostBallEffect != NULL) {
		delete this->ghostBallEffect;
		this->ghostBallEffect = NULL;
	}

	if (this->fireBallEffect != NULL) {
		delete this->fireBallEffect;
		this->fireBallEffect = NULL;
	}
    
    if (this->omniLaserBallEffect != NULL) {
        delete this->omniLaserBallEffect;
        this->omniLaserBallEffect = NULL;
    }

    if (this->magnetPaddleEffect != NULL) {
        delete this->magnetPaddleEffect;
        this->magnetPaddleEffect = NULL;
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

        case Projectile::BossLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
            this->sound->PlaySoundAtPosition(GameSound::LaserBulletShotEvent, false, projectile.GetPosition3D());
            break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            assert(dynamic_cast<const RocketProjectile*>(&projectile) != NULL);
			// Notify assets of the rocket...
			this->FireRocket(*static_cast<const RocketProjectile*>(&projectile));
			break;

        case Projectile::PaddleRemoteCtrlRocketBulletProjectile: {
            assert(dynamic_cast<const PaddleRemoteControlRocketProjectile*>(&projectile) != NULL);
           
            const PaddleRemoteControlRocketProjectile* remoteCtrlRocket = static_cast<const PaddleRemoteControlRocketProjectile*>(&projectile);
            
            // Activate the remote control rocket HUD
            this->remoteControlRocketHUD->Activate(remoteCtrlRocket);

            // Notify assets of the rocket...
            this->FireRocket(*remoteCtrlRocket);
            break;
        }

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile: {
            // Notify the mine manager...
            const MineProjectile* mine = static_cast<const MineProjectile*>(&projectile);
            this->mineMeshMgr->AddMineProjectile(mine);
            break;
        }

		default:
			break;
	}
}

/** 
 * Remove the given projectile and its effects from the assets.
 */
void GameAssets::RemoveProjectile(const Projectile& projectile) {
	this->espAssets->RemoveProjectileEffect(projectile);
    
    // Kill all sounds for the projectile
    this->sound->DetachAndStopAllSounds(&projectile);

	switch (projectile.GetType()) {

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            assert(dynamic_cast<const RocketProjectile*>(&projectile) != NULL);
			this->rocketMesh->DeactivateRocket(static_cast<const RocketProjectile*>(&projectile));
			break;
        
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
            // Deactivate the rocket HUD
            this->remoteControlRocketHUD->Deactivate();

            assert(dynamic_cast<const RocketProjectile*>(&projectile) != NULL);
            this->rocketMesh->DeactivateRocket(static_cast<const RocketProjectile*>(&projectile));
            break;

        case Projectile::FireGlobProjectile:
            this->espAssets->AddFireGlobDestroyedEffect(projectile);
            break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile: {
            // Notify the mine manager...
            const MineProjectile* mine = static_cast<const MineProjectile*>(&projectile);
            this->mineMeshMgr->RemoveMineProjectile(mine);
            break;
        }

		default:
			break;
	}
}

/**
 * Private helper, cause the laser paddle attachment to animate - like it's reacting to shooting
 * a bullet or something.
 */
void GameAssets::FirePaddleLaser(const PlayerPaddle& paddle) {
    // Add a sound for firing the laser
    this->sound->PlaySoundAtPosition(GameSound::LaserBulletShotEvent, false, paddle.GetPosition3D());
    // Make the laser attachment animate for firing the gun...
	this->paddleLaserAttachment->FirePaddleLaserGun(paddle);
}

/**
 * Private helper, causes the rocket to be fired and drawn.
 */
void GameAssets::FireRocket(const RocketProjectile& rocketProjectile) {
    // Play a sound for a rocket firing and travelling (based off the type)
    switch (rocketProjectile.GetType()) {

        case Projectile::BossRocketBulletProjectile:
            // TODO?
            break;
        
        case Projectile::RocketTurretBulletProjectile:
            // TODO?
            break;

        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::PaddleRocketBulletProjectile:
            // The rocket launch sound
            this->sound->PlaySoundAtPosition(GameSound::PaddleRocketLaunchEvent, false, rocketProjectile.GetPosition3D());
            // Attach a moving sound loop to the rocket
            this->sound->AttachAndPlaySound(&rocketProjectile, GameSound::PaddleRocketMovingLoop, true);
            break;

        default:
            assert(false);
            break;
    }

	this->rocketMesh->ActivateRocket(&rocketProjectile);
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

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			intensity = PlayerHurtHUD::MinorPain;
			break;

        case Projectile::RocketTurretBulletProjectile:
        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            intensity = PlayerHurtHUD::ModeratePain;
            break;

        case Projectile::CollateralBlockProjectile:
		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
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

	// Add a full screen overlay effect to show pain/badness
	this->painHUD->Activate(intensity);
}

void GameAssets::PaddleHurtByBeam(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment) {
    UNUSED_PARAMETER(beam);

    // Add the sprite/particle effects
    this->espAssets->AddPaddleHitByBeamEffect(paddle, beamSegment);

    // Add a full screen overlay effect to show pain/badness
    this->painHUD->Activate(PlayerHurtHUD::MajorPain);

    // Vibrate the controller
    float intensityMultiplier = (beamSegment.GetRadius() / paddle.GetHalfWidthTotal());
    GameControllerManager::GetInstance()->VibrateControllers(intensityMultiplier * 1.5f,
        BBBGameController::HeavyVibration, BBBGameController::MediumVibration);
}

// Notifies assets that there was an explosion and there should be a fullscreen flash
void GameAssets::RocketExplosion(const RocketProjectile& rocket, Camera& camera) {
    float forcePercentage = rocket.GetForcePercentageFactor();
    this->espAssets->AddRocketBlastEffect(forcePercentage, rocket.GetPosition());

	// Add a camera/controller shake and flash for when the rocket explodes...
    
    // Make boss rockets a bit weaker (interferes a bit less with the gameplay during the boss fight)
    if (rocket.GetType() == Projectile::BossRocketBulletProjectile) {
        camera.SetCameraShake(forcePercentage * 0.75f, forcePercentage * Vector3D(0.7f, 0.6f, 0.1f), 100);
        GameControllerManager::GetInstance()->VibrateControllers(forcePercentage * 1.2f,
            BBBGameController::SoftVibration, BBBGameController::MediumVibration);
        this->flashHUD->Activate(0.33, 0.75f);
    }
    else {
        camera.SetCameraShake(forcePercentage * 1.2f, forcePercentage * Vector3D(0.9f, 0.8f, 0.1f), 130);
        GameControllerManager::GetInstance()->VibrateControllers(forcePercentage * 1.2f,
            BBBGameController::MediumVibration, BBBGameController::HeavyVibration);
        this->flashHUD->Activate(0.5, 1.0f);
    }

	// Play the explosion sound
	this->sound->PlaySoundAtPosition(GameSound::RocketExplodedEvent, false, Point3D(rocket.GetPosition(), 0.0f));
}

void GameAssets::MineExplosion(const MineProjectile& mine, Camera& camera) {
    this->espAssets->AddMineBlastEffect(mine, mine.GetPosition());

	// Add a camera/controller shake and flash for when the mine explodes...
    float forcePercentage = mine.GetVisualScaleFactor();
	camera.SetCameraShake(forcePercentage * 0.5f, forcePercentage * Vector3D(0.2f, 0.5f, 0.3f), 80);
    GameControllerManager::GetInstance()->VibrateControllers(forcePercentage * 0.75f,
        BBBGameController::VerySoftVibration, BBBGameController::SoftVibration);

	// Play the explosion sound
	this->sound->PlaySoundAtPosition(GameSound::MineExplodedEvent, false, Point3D(mine.GetPosition(), 0.0f));
}

void GameAssets::FullscreenFlashExplosion(const FullscreenFlashEffectInfo& info, Camera& camera) {
	// Add a camera/controller shake and fullscreen flash
    if (info.GetShakeMultiplier() > 0.0f) {
        double shakeTime = 0.9 * info.GetTime();
        camera.SetCameraShake(shakeTime, info.GetShakeMultiplier() * Vector3D(0.2f, 0.5f, 0.3f), 100);
        GameControllerManager::GetInstance()->VibrateControllers(shakeTime,
            BBBGameController::MediumVibration, BBBGameController::MediumVibration);
    }
    
    this->flashHUD->Activate(info.GetTime(), 1.0f);
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
	LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading movement assets...");
	if (this->worldAssets == NULL || this->worldAssets->GetStyle() != world.GetStyle()) {
		// Delete all previously loaded style-related assets
		delete this->worldAssets;
		this->worldAssets = NULL;

		// Load up the new set of world geometry assets
		this->worldAssets = GameWorldAssets::CreateWorldAssets(world.GetStyle(), this);
		assert(this->worldAssets != NULL);
	}

	LoadingScreen::GetInstance()->UpdateLoadingScreen(LoadingScreen::ABSURD_LOADING_DESCRIPTION);
    this->ReinitializeAssets();
}

void GameAssets::ReinitializeAssets() {
	this->worldAssets->ResetToInitialState();

	// Reinitialize the life HUD elements
	this->lifeHUD->Reinitialize();
    this->pointsHUD->Reinitialize();
    this->boostHUD->Reinitialize();
    this->ballReleaseHUD->Reinitialize();
    this->remoteControlRocketHUD->Reinitialize();
}

void GameAssets::ActivateRandomItemEffects(const GameModel& gameModel, const GameItem& actualItem) {
	// Figure out the item texture associated with the actual item...
	Texture2D* actualItemTexture = this->itemAssets->GetItemTexture(actualItem.GetItemType());
	// This will start and animation that shows to the player what the random item has become
    this->randomToItemAnimation.Start(actualItem, actualItemTexture, gameModel, this->espAssets);
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

        case GameItem::LifeUpItem:
            this->sound->PlaySound(GameSound::LifeUpAcquiredEvent, false);
            break;

		case GameItem::BlackoutItem: {
			// Turn the lights off and make only the paddle and ball visible.
			assert(gameModel.IsBlackoutEffectActive());
			this->lightAssets->ToggleLights(false, GameLightAssets::DEFAULT_LIGHT_TOGGLE_TIME);
			break;
        }
			
		case GameItem::PoisonPaddleItem: {
			// The poison item will make the lights turn a sickly green colour
			assert(gameModel.GetPlayerPaddle()->HasPaddleType(PlayerPaddle::PoisonPaddle));

			this->lightAssets->StartStrobeLight(GameLightAssets::FGKeyLight, GameViewConstants::GetInstance()->POISON_LIGHT_LIGHT_COLOUR, 1.0f);
			this->lightAssets->StartStrobeLight(GameLightAssets::FGFillLight, GameViewConstants::GetInstance()->POISON_LIGHT_DEEP_COLOUR, 1.0f);
			this->lightAssets->StartStrobeLight(GameLightAssets::BallKeyLight, GameViewConstants::GetInstance()->POISON_LIGHT_LIGHT_COLOUR, 1.0f);

            // Apply the sound effect for poison...
            this->sound->ToggleSoundEffect(GameSound::PoisonEffect, true);
            break;
		}

		case GameItem::PaddleCamItem: {
			// For the paddle camera we remove the stars from all currently falling items (block the view of the player)
			// NOTE that this is not permanent and just does so for any currently falling items
			this->espAssets->TurnOffCurrentItemDropStars();
			
			// We make the safety net transparent so that it doesn't obstruct the paddle cam... too much

            // Call to make the safety net visible or not when its activated. This is
            // useful to avoid obstruction of the player's viewpoint e.g., when in paddle camera mode.
            this->ballSafetyNet->SetAlpha(0.5f);

			// Move the key light in the foreground so that it is behind the camera when it goes into
			// paddle cam mode.
			float halfLevelHeight = gameModel.GetCurrentLevel()->GetLevelUnitHeight() / 2.0f;
			float halfLevelWidth  = gameModel.GetCurrentLevel()->GetLevelUnitWidth() / 2.0f;

			Point3D newFGKeyLightPos(halfLevelWidth, -(halfLevelHeight + 10.0f), 10.0f);
			Point3D newFGFillLightPos(-halfLevelWidth, -(halfLevelHeight + 10.0f), -5.0f);
			
			this->lightAssets->ChangeLightPositionAndAttenuation(GameLightAssets::FGKeyLight, newFGKeyLightPos, 0.08f, 1.5f);
			this->lightAssets->ChangeLightPositionAndAttenuation(GameLightAssets::FGFillLight, newFGFillLightPos, 0.12f, 1.5f);

			// Fade out the background...
			this->worldAssets->FadeBackground(true, 2.0f);
            break;
		}

		case GameItem::BallCamItem: {
			// For the paddle camera we remove the stars from all currently falling items (block the view of the player)
			// NOTE that this is not permanent and just does so for any currently falling items
			this->espAssets->TurnOffCurrentItemDropStars();

			// Change the position of the key light so that it is facing down with the ball
			float halfLevelHeight = gameModel.GetCurrentLevel()->GetLevelUnitHeight() / 2.0f;

			Point3D newFGKeyLightPos(0.0f, (halfLevelHeight + 10.0f), -5.0f);
			Point3D newFGFillLightPos(0.0f, -(halfLevelHeight + 10.0f), -10.0f);
			
			this->lightAssets->ChangeLightPositionAndAttenuation(GameLightAssets::FGKeyLight, newFGKeyLightPos, 0.08f, 1.5f);
			this->lightAssets->ChangeLightPositionAndAttenuation(GameLightAssets::FGFillLight, newFGFillLightPos, 0.12f, 1.5f);

			// Fade out the background...
			this->worldAssets->FadeBackground(true, 2.0f);
            break;
        }



		case GameItem::ShieldPaddleItem: {

            this->sound->PlaySoundAtPosition(GameSound::PaddleShieldActivatedEvent, false, 
                gameModel.GetPlayerPaddle()->GetPosition3D());

            const Texture2D* fullscreenFBOTex = this->GetFBOAssets()->GetFullSceneFBO()->GetFBOTexture();
			assert(fullscreenFBOTex != NULL);
			this->paddleShield->ActivateShield(*gameModel.GetPlayerPaddle(), *fullscreenFBOTex);
            break;
        }

        case GameItem::MagnetPaddleItem:
            this->sound->AttachAndPlaySound(gameModel.GetPlayerPaddle(), GameSound::MagnetPaddleLoop, true);
            this->magnetPaddleEffect->Reset();
            break;

        case GameItem::UpsideDownItem:
            this->sound->PlaySound(GameSound::LevelFlipEvent, false);
            break;

        case GameItem::BallGrowItem:
            this->sound->PlaySound(GameSound::BallOrPaddleGrowEvent, false);
            break;
        case GameItem::PaddleGrowItem:
            this->sound->PlaySound(GameSound::BallOrPaddleGrowEvent, false);
            break;

        case GameItem::BallShrinkItem:
            this->sound->PlaySound(GameSound::BallOrPaddleShrinkEvent, false);
            break;
        case GameItem::PaddleShrinkItem:
            this->sound->PlaySound(GameSound::BallOrPaddleShrinkEvent, false);
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
    UNUSED_PARAMETER(gameModel);

	// Also make the FBO assets aware of the deactivated effect
	this->fboAssets->DeactivateItemEffects(item);

	// Deal with any light changes...
	switch (item.GetItemType()) {
		case GameItem::BlackoutItem: {
			// Turn the lights back on and revert lights back to their defaults
			assert(!gameModel.IsBlackoutEffectActive());
			this->lightAssets->ToggleLights(true, GameLightAssets::DEFAULT_LIGHT_TOGGLE_TIME);
            break;
        }

		case GameItem::PoisonPaddleItem: {
			// Stop strobing the lights
			this->lightAssets->StopStrobeLight(GameLightAssets::FGKeyLight);
			this->lightAssets->StopStrobeLight(GameLightAssets::FGFillLight);
			this->lightAssets->StopStrobeLight(GameLightAssets::BallKeyLight);

            // Deactivate sound effects
            this->sound->ToggleSoundEffect(GameSound::PoisonEffect, false);
            break;
		}

		case GameItem::PaddleCamItem: {
			// We make the safety net visible (if activated) again
			this->ballSafetyNet->SetAlpha(1.0f);

			// Move the foreground key and fill lights back to their default positions...
			this->lightAssets->RestoreLightPositionAndAttenuation(GameLightAssets::FGKeyLight, 1.5f);
			this->lightAssets->RestoreLightPositionAndAttenuation(GameLightAssets::FGFillLight, 1.5f);

			// Show the background once again...
			this->worldAssets->FadeBackground(false, 2.0f);
            break;
        }

		case GameItem::BallCamItem: 
			// Move the foreground key and fill lights back to their default positions...
			this->lightAssets->RestoreLightPositionAndAttenuation(GameLightAssets::FGKeyLight, 1.5f);
			this->lightAssets->RestoreLightPositionAndAttenuation(GameLightAssets::FGFillLight, 1.5f);

			// Show the background once again...
			this->worldAssets->FadeBackground(false, 2.0f);
			break;

		case GameItem::ShieldPaddleItem:
            this->sound->PlaySoundAtPosition(GameSound::PaddleShieldDeactivatedEvent, false, 
                gameModel.GetPlayerPaddle()->GetPosition3D());
			this->paddleShield->DeactivateShield();
            break;

        case GameItem::UpsideDownItem:
            this->sound->PlaySound(GameSound::LevelUnflipEvent, false);
            break;

        case GameItem::MagnetPaddleItem:
            this->sound->DetachAndStopSound(gameModel.GetPlayerPaddle(), GameSound::MagnetPaddleLoop);
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
	this->espAssets->KillAllActiveEffects(true);
	this->espAssets->KillAllActiveTeslaLightningArcs();
	this->itemAssets->ClearTimers();
	// Disable any overlay stuff
	this->painHUD->Deactivate();
    this->flashHUD->Deactivate();
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
    //this->fboAssets->
}

/**
 * Deactivate any effects that need to be deactivated from when the last
 * ball was dieing.
 */
void GameAssets::DeactivateLastBallDeathEffects() {
	// Bring the world background back into view
	this->worldAssets->FadeBackground(false, 2.0f);
}

void GameAssets::ApplyRocketThrust(const PaddleRemoteControlRocketProjectile& rocket) {
    this->rocketMesh->ApplyRocketThrust(rocket);
}