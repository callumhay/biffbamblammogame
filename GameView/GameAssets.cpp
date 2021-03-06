/**
 * GameAssets.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "GameAssets.h"
#include "GameESPAssets.h"
#include "GameFBOAssets.h"
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
#include "MalfunctionTextHUD.h"
#include "BallCamHUD.h"
#include "PaddleCamHUD.h"
#include "ButtonTutorialHint.h"
#include "StickyPaddleGoo.h"
#include "PaddleGunAttachment.h"
#include "RocketMesh.h"
#include "PaddleShield.h"
#include "OmniLaserBallEffect.h"
#include "MagnetPaddleEffect.h"
#include "MineMeshManager.h"
#include "PaddleMineLauncher.h"
#include "PaddleBeamAttachment.h"
#include "PaddleStatusEffectRenderer.h"

// Game Model includes
#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/Beam.h"
#include "../GameModel/BossLaserBeam.h"
#include "../GameModel/LaserBulletProjectile.h"
#include "../GameModel/PaddleRocketProjectile.h"
#include "../GameModel/FireGlobProjectile.h"
#include "../GameModel/PaddleMineProjectile.h"
#include "../GameModel/PaddleRemoteControlRocketProjectile.h"
#include "../GameModel/PaddleFlameBlasterProjectile.h"
#include "../GameModel/FullscreenFlashEffectInfo.h"
#include "../GameModel/GameTransformMgr.h"
#include "../GameModel/CannonBlock.h"

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

ballCamHUD(NULL),
paddleCamHUD(NULL),

skipLabel(NULL),

ball(NULL), 
spikeyBall(NULL),
rocketMesh(NULL),
mineMeshMgr(NULL),
paddleBeamAttachment(NULL),
paddleMineAttachment(NULL),
paddleGunAttachment(NULL),
paddleStickyAttachment(NULL),
paddleShield(NULL),
paddleStatusEffectRenderer(NULL),
bottomSafetyNetMesh(NULL),
topSafetyNetMesh(NULL),

invisibleEffect(NULL), 
ghostBallEffect(NULL),
fireBallEffect(NULL),

omniLaserBallEffect(NULL),
magnetPaddleEffect(NULL)
{
	// Load ESP assets
	LoadingScreen::GetInstance()->UpdateLoadingScreenWithRandomLoadStr();
	this->espAssets      = new GameESPAssets();
    this->tutorialAssets = new GameTutorialAssets();

	// Load FBO assets
    this->fboAssets = new GameFBOAssets(screenWidth, screenHeight, this->sound);

	// Load item assets
	LoadingScreen::GetInstance()->UpdateLoadingScreenWithRandomLoadStr();
	this->itemAssets = new GameItemAssets(this->espAssets, this->sound);
	bool didItemAssetsLoad = this->itemAssets->LoadItemAssets();
    UNUSED_VARIABLE(didItemAssetsLoad);
	assert(didItemAssetsLoad);

	// Load all fonts
	LoadingScreen::GetInstance()->UpdateLoadingScreenWithRandomLoadStr();
	GameFontAssetsManager::GetInstance()->LoadMinimalFonts();

	// Load regular meshes
	LoadingScreen::GetInstance()->UpdateLoadingScreenWithRandomLoadStr();
	this->LoadRegularMeshAssets();

	// Load regular effects
	LoadingScreen::GetInstance()->UpdateLoadingScreenWithRandomLoadStr();
	this->LoadRegularEffectAssets();

	// Initialize any HUD elements
	this->crosshairHUD	 = new CrosshairLaserHUD();
	this->lifeHUD        = new LivesLeftHUD();
	this->painHUD        = new PlayerHurtHUD();
	this->flashHUD       = new FlashHUD();
    this->pointsHUD      = new PointsHUD();
    this->boostHUD       = new BallBoostHUD(screenHeight);
    this->ballReleaseHUD = new BallReleaseHUD(this->sound);
    this->remoteControlRocketHUD = new RemoteControlRocketHUD(*this);

    this->ballCamHUD   = new BallCamHUD(*this);
    this->paddleCamHUD = new PaddleCamHUD(*this);

    this->skipLabel = new ButtonTutorialHint(this->tutorialAssets, "Skip");
    if (GameDisplay::IsArcadeModeEnabled()) {
        this->skipLabel->SetArcadeButton(GameViewConstants::ArcadeFireButton, "Fire", 
            GameViewConstants::GetInstance()->ARCADE_FIRE_BUTTON_COLOUR);
    }
    else {
        this->skipLabel->SetXBoxButton(GameViewConstants::XBoxPushButton, "A", GameViewConstants::GetInstance()->XBOX_CONTROLLER_A_BUTTON_COLOUR);
        this->skipLabel->SetKeyboardButton(GameViewConstants::KeyboardSpaceBar, "Space");
    }

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
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->ball);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->spikeyBall);
    assert(success);

    UNUSED_VARIABLE(success);

	delete this->rocketMesh;
	this->rocketMesh = NULL;
    delete this->mineMeshMgr;
    this->mineMeshMgr = NULL;
	delete this->paddleGunAttachment;
	this->paddleGunAttachment = NULL;
	delete this->paddleStickyAttachment;
	this->paddleStickyAttachment = NULL;
	delete this->paddleShield;
	this->paddleShield = NULL;

    delete this->paddleStatusEffectRenderer;
    this->paddleStatusEffectRenderer = NULL;

    delete this->paddleBeamAttachment;
    this->paddleBeamAttachment = NULL;
    delete this->paddleMineAttachment;
    this->paddleMineAttachment = NULL;

    delete this->bottomSafetyNetMesh;
    this->bottomSafetyNetMesh = NULL;
    delete this->topSafetyNetMesh;
    this->topSafetyNetMesh = NULL;

    // Clean up tutorial assets
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
    
    delete this->ballCamHUD;
    this->ballCamHUD = NULL;
    delete this->paddleCamHUD;
    this->paddleCamHUD = NULL;

    delete this->skipLabel;
    this->skipLabel = NULL;
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
	Vector3D avgBallColourVec(0,0,0);

	int visibleBallCount        = 0;
    bool ballWithOmniLaserDrawn = false;

    // Deal with the special case of the ball camera...
    const GameBall* camBall = GameBall::GetBallCameraBall();
    if (camBall == NULL) {
        // This will draw any affects that occur right after a boost takes place
        this->espAssets->DrawBallBoostingEffects(dT, camera);
    }

	// Go through each ball in the game, draw it accordingly
	const std::list<GameBall*>& balls = gameModel.GetGameBalls();
	for (std::list<GameBall*>::const_iterator ballIter = balls.begin(); ballIter != balls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		
        // Obtain the colour of the ball from the model, if it's completely invisible 
        // (NOT "invisiball", actually invisible), then just skip all of the draw calls, unless we're
        // in ball camera mode, in which case we still want to light things up
        const ColourRGBA& ballColour = currBall->GetColour();
        if (ballColour.A() <= 0.0f && !currBall->HasBallCameraActive()) {
            continue;
        }

		bool ballIsInvisible = (currBall->GetBallType() & GameBall::InvisiBall) == GameBall::InvisiBall;
		CgFxEffectBase* ballEffectTemp = NULL;
		Vector3D currBallColourVec(0,0,0);

		const Point2D& ballPos = currBall->GetCenterPosition2D();

		if (currBall->GetBallType() == GameBall::NormalBall) {
			// Normal ball with a regular light
			avgBallPosition += Vector3D(ballPos[0], ballPos[1], 0);
			avgBallColourVec   += GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_COLOUR.GetAsVector3D();
			visibleBallCount++;
		}
		else {
			// The ball has an item in effect on it... figure out what effect(s) and render it/them appropriately

            if (ballIsInvisible) {
                // INVISIBALL:
                // Obtain the POST full screen rendering from the previous frame - this is
                // a bit of a hack but it saves us from reading/writing to the same FBO simultaneously
                this->invisibleEffect->SetFBOTexture(this->fboAssets->GetFullSceneFBO()->GetFBOTexture());
                ballEffectTemp = this->invisibleEffect;
            }
            // We only apply special colour/misc. effects if the ball camera is not on OR
            // if the ball camera is on and the current ball has the camera
            else if (!GameBall::GetIsBallCameraOn() || currBall->HasBallCameraActive()) {
                int numColoursApplied = 0;

                // SLOW AND FAST BALL CHECK
                if ((currBall->GetBallType() & GameBall::SlowBall) == GameBall::SlowBall) {
				    // We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
                        this->espAssets->DrawSlowBallEffects(dT, camera, *currBall);
				    }
                    currBallColourVec += GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_COLOUR.GetAsVector3D();
                    numColoursApplied++;
                }
                else if ((currBall->GetBallType() & GameBall::FastBall) == GameBall::FastBall) {
				    // We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
                        this->espAssets->DrawFastBallEffects(dT, camera, *currBall);
				    }
                    currBallColourVec += GameViewConstants::GetInstance()->DEFAULT_BALL_LIGHT_COLOUR.GetAsVector3D();
                    numColoursApplied++;
                }

			    // GHOST BALL CHECK
			    if ((currBall->GetBallType() & GameBall::GhostBall) == GameBall::GhostBall) {
				    // We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
					    this->espAssets->DrawGhostBallEffects(dT, camera, *currBall);
				    }
    				
                    ballEffectTemp = this->ghostBallEffect;
				    currBallColourVec += GameModelConstants::GetInstance()->GHOST_BALL_COLOUR.GetAsVector3D();
				    numColoursApplied++;
			    }

			    // UBER BALL CHECK
			    if ((currBall->GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
				    // Draw when uber ball and not invisiball...
				    // We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
					    this->espAssets->DrawUberBallEffects(dT, camera, *currBall);
				    }

				    currBallColourVec += GameModelConstants::GetInstance()->UBER_BALL_COLOUR.GetAsVector3D();
				    numColoursApplied++;
			    }

			    // GRAVITY BALL CHECK
			    if ((currBall->GetBallType() & GameBall::GraviBall) == GameBall::GraviBall) {
				    // Draw when gravity ball and not invisiball...
				    // We don't draw any of the effects if we're in ball camera mode or the ball is inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
                       // if (!gameModel.GetPlayerPaddle()->GetIsPaddleCameraOn()) {
					        Vector3D gravityDir = gameModel.GetTransformInfo()->GetGameXYZTransform() * Vector3D(0, -1, 0);
					        this->espAssets->DrawGravityBallEffects(dT, camera, *currBall, gravityDir);
                       // }
				    }
    				
				    currBallColourVec += GameModelConstants::GetInstance()->GRAVITY_BALL_COLOUR.GetAsVector3D();
				    numColoursApplied++;
			    }

			    // CRAZY BALL CHECK
			    if ((currBall->GetBallType() & GameBall::CrazyBall) == GameBall::CrazyBall) {
				    // Draw when crazy ball when not invisible...
				    // We don't draw any of the effects if we're in ball camera mode
				    if (!GameBall::GetIsBallCameraOn()) {				
					    this->espAssets->DrawCrazyBallEffects(dT, camera, *currBall);
				    }
    				
				    currBallColourVec += GameModelConstants::GetInstance()->CRAZY_BALL_COLOUR.GetAsVector3D();
				    numColoursApplied++;
			    }

			    // FIRE BALL CHECK
			    if ((currBall->GetBallType() & GameBall::FireBall) == GameBall::FireBall) {
				    // Draw the fire ball when not invisible
				    // We don't draw any of the effects if we're in a ball camera mode or inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {				
					    this->espAssets->DrawFireBallEffects(dT, camera, *currBall);
				    }
                    ballEffectTemp = this->fireBallEffect;
				    currBallColourVec += GameModelConstants::GetInstance()->FIRE_BALL_COLOUR.GetAsVector3D();
				    numColoursApplied++;
			    }

			    // ICE BALL CHECK
			    if ((currBall->GetBallType() & GameBall::IceBall) == GameBall::IceBall) {
				    // Draw the ice ball when not invisible
				    // We don't draw any of the effects if we're in a ball camera mode or inside a cannon
				    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
					    this->espAssets->DrawIceBallEffects(dT, camera, *currBall);
				    }
				    currBallColourVec += GameModelConstants::GetInstance()->ICE_BALL_COLOUR.GetAsVector3D();
				    numColoursApplied++;
			    }

                // OMNI LASER BALL CHECK
                if ((currBall->GetBallType() & GameBall::OmniLaserBulletBall) == GameBall::OmniLaserBulletBall) {
                    if (!GameBall::GetIsBallCameraOn() && !currBall->IsLoadedInCannonBlock()) {
					    this->omniLaserBallEffect->Draw(dT, !ballWithOmniLaserDrawn, camera, *currBall);
                        ballWithOmniLaserDrawn = true;
				    }
                    currBallColourVec += GameModelConstants::GetInstance()->OMNI_LASER_BALL_COLOUR.GetAsVector3D();
                    numColoursApplied++;
                }

				// We only take the average of visible balls.
				avgBallPosition = avgBallPosition + Vector3D(ballPos[0], ballPos[1], 0);
			
				numColoursApplied = std::max<int>(1, numColoursApplied);
				avgBallColourVec += (currBallColourVec / numColoursApplied);

				visibleBallCount++;
            }
		}

        // If the ball isn't visible then just skip all of the draw calls
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
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT);
		if (ballIsInvisible || ballColour.A() < 1.0f) {
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

    if (camBall == NULL) {
        // If the ball boost direction is being decided then we need to draw the boost effect
        this->DrawGameBallsBoostPostEffects(dT, gameModel, camera);

	    // Calculate the average position and colour of all the visible balls in the game
	    if (visibleBallCount > 0) {
	        avgBallPosition  /= static_cast<float>(visibleBallCount);
	        avgBallPosition  += Vector3D(worldT[0], worldT[1], 0.0f);
	        avgBallColourVec /= static_cast<float>(visibleBallCount);
    		
	        // Grab a transform matrix from the game model to say where the ball light is
	        // if the level is flipped or some such thing
	        avgBallPosition = gameModel.GetTransformInfo()->GetGameXYZTransform() * avgBallPosition;

            // Set the ball light to the correct position
            this->lightAssets->GetBallLight().SetPosition(avgBallPosition);
	    }
	    else {
		    avgBallColourVec = Vector3D(0,0,0);
	    }
    }
    else {
        // No averaging of position or colour and we ignore anything concerning "visible ball counts", 
        // we want the ball camera to have good illumination at all times or it will drive players
        // even more insane than it already does
        avgBallPosition  = camBall->GetCenterPosition();
        avgBallPosition += Vector3D(worldT[0], worldT[1], 0.0f);
        avgBallPosition  = gameModel.GetTransformInfo()->GetGameXYZTransform() * avgBallPosition;
        
        // Set the FG key light to the same position as the ball...    
        this->lightAssets->GetFGKeyLight().SetPosition(avgBallPosition);

        // Set the ball light to the correct position
        this->lightAssets->GetBallLight().SetPosition(avgBallPosition);
    }

	// Set the ball light to the correct diffuse colour
	this->lightAssets->GetBallLight().SetDiffuseColour(Colour(avgBallColourVec));
	debug_opengl_state();
}

/**
 * Draw the effects that take place after drawing everything (Except final full screen effects).
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

void GameAssets::Tick(double dT, const GameModel& gameModel) {
	// Tick the world assets (e.g., for background animations)
	this->worldAssets->Tick(dT, gameModel);

	// Tick the FBO assets (e.g., for post-processing / full screen effects animations)
	this->fboAssets->Tick(dT);

	// Tick the light assets (light animations for strobing, changing colours, etc.)
	this->lightAssets->Tick(dT);

    // Update the ESP assets
    this->espAssets->Update(gameModel);

	// Update the esp assets with any changes over the last tick
	this->espAssets->UpdateBGTexture(*this->fboAssets->GetFinalFullScreenFBO()->GetFBOTexture());
}

/**
 * Draw the player paddle mesh with materials and in correct position.
 */
void GameAssets::DrawPaddle(double dT, const GameModel& gameModel, const Camera& camera) {
	
    const PlayerPaddle& p = *gameModel.GetPlayerPaddle();
    const Point2D& paddleCenter = p.GetCenterPosition();	
	float paddleScaleFactor     = p.GetPaddleScaleFactor();
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
        this->invisibleEffect->SetFBOTexture(this->fboAssets->GetFinalFullScreenFBO()->GetFBOTexture());
        paddleReplacementMat = this->invisibleEffect;
        // Just tick the effects on the paddle, no drawing
        this->espAssets->TickButDontDrawBackgroundPaddleEffects(dT);
    }
    else {
        paddleReplacementMat = this->paddleStatusEffectRenderer->GetPaddleStatusMaterial();
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
		else if (!p.HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle)){
			// Draw glow-y beam origin when beam is able to fire but not actually firing yet
			this->espAssets->DrawPaddleLaserBeamBeforeFiringEffects(dT, camera, p);
		}
	}

	// In the case of a laser bullet paddle (and NOT paddle camera mode), we draw the laser attachment and its related effects
	// Camera mode is exempt from this because the attachment would seriously get in the view of the player
    if (!p.GetIsPaddleCameraOn() && p.GetAlpha() > 0.0f &&
         p.HasPaddleType(PlayerPaddle::LaserBulletPaddle | PlayerPaddle::LaserBeamPaddle | 
         PlayerPaddle::MineLauncherPaddle | PlayerPaddle::FlameBlasterPaddle | PlayerPaddle::IceBlasterPaddle)) {

		glColor4f(1.0f, 1.0f, 1.0f, p.GetAlpha());

        glPushAttrib(GL_DEPTH_BUFFER_BIT);
        if (p.HasPaddleType(PlayerPaddle::InvisiPaddle) || p.GetAlpha() < 1.0f) {
            // Disable the depth draw, this will make sure the paddle attachments don't get outlined
            glDepthMask(GL_FALSE);
        }

        // Draw the various active attachments...
        if (p.HasPaddleType(PlayerPaddle::LaserBulletPaddle | PlayerPaddle::FlameBlasterPaddle | PlayerPaddle::IceBlasterPaddle)) {
			this->paddleGunAttachment->Draw(dT, p, camera, paddleReplacementMat, paddleKeyLight, paddleFillLight, ballLight);
		}

        // NOTE: This transform only works because the meshes for the attachements are already in paddle space
        glPushMatrix();
        glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
        glRotatef(p.GetZRotation(), 0, 0, 1);
       
		if (p.HasPaddleType(PlayerPaddle::LaserBeamPaddle)) {
			this->paddleBeamAttachment->Draw(p, camera, paddleReplacementMat, paddleKeyLight, paddleFillLight, ballLight);
		}
        if (p.HasPaddleType(PlayerPaddle::MineLauncherPaddle)) {
            this->mineMeshMgr->DrawLoadingMine(dT, p, camera, paddleKeyLight, paddleFillLight, ballLight,
                this->fboAssets->GetFullSceneFBO()->GetFBOTexture());
			this->paddleMineAttachment->Draw(dT, p, camera, paddleReplacementMat, paddleKeyLight, paddleFillLight, ballLight);
		}

        glPopMatrix();
        glPopAttrib();
	}
    else if (p.GetIsPaddleCameraOn()) {
        // When the paddle camera is on we want to move the foreground key light around with the paddle

        Point3D keyLightPos(p.GetCenterPosition(), 0.0f);
        keyLightPos[1] += PaddleCamHUD::PADDLE_CAM_FG_KEY_LIGHT_Y_OFFSET;
        keyLightPos += gameModel.GetCurrentLevelTranslation();
        keyLightPos  = gameModel.GetTransformInfo()->GetGameXYZTransform() * keyLightPos;

        this->lightAssets->GetFGKeyLight().SetPosition(keyLightPos);
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
    // NOTE: IF YOU CHANGE THIS THEN RE-EXAMINE ALL THE CALLS AFTER IT, BECAUSE THEY RELY
    // ON THE PADDLE CAMERA NOT BEING ON!!!
	if (paddle->GetIsPaddleCameraOn() || paddle->GetAlpha() <= 0.0f) {
		return;
	}
	
	const Point2D& paddleCenter = paddle->GetCenterPosition();
	float paddleScaleFactor     = paddle->GetPaddleScaleFactor();
	float scaleHeightAdjustment = PlayerPaddle::PADDLE_HALF_HEIGHT * (paddleScaleFactor - 1);

	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	glTranslatef(paddleCenter[0], paddleCenter[1] + scaleHeightAdjustment, 0);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	

	// Draw the shield around the paddle (if it's currently activated)
    this->paddleShield->SetAlpha(paddle->GetAlpha());
	this->paddleShield->DrawAndTick(*paddle, camera, dT);

	// When the paddle has the 'sticky' power-up we attach sticky goo to its top
	if (paddle->HasPaddleType(PlayerPaddle::StickyPaddle)) {

		// Set the texture for the refraction in the goo - be careful here, we can't get
		// the 'post full scene' FBO because we are currently in the middle of drawing to it
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
	if (paddle->HasPaddleType(PlayerPaddle::LaserBulletPaddle) && 
        !paddle->HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle)) {

		// Draw glow-y effects where the laser originates...
		this->espAssets->DrawPaddleLaserBulletEffects(dT, camera, *paddle);
	}

    if (paddle->HasPaddleType(PlayerPaddle::FlameBlasterPaddle)) {
        this->espAssets->DrawPaddleFlameBlasterEffects(dT, camera, *paddle);
    }
    else if (paddle->HasPaddleType(PlayerPaddle::IceBlasterPaddle)) {
        this->espAssets->DrawPaddleIceBlasterEffects(dT, camera, *paddle);
    }

    // Status effects for the paddle...
    this->paddleStatusEffectRenderer->Draw(dT, camera, *paddle, sceneFBO->GetFBOTexture());

    glPopMatrix();

	// BALL CAMERA CHECK
	if (GameBall::GetIsBallCameraOn() && !gameModel.IsBlackoutEffectActive()) {
		const GameBall* ballWithCam = GameBall::GetBallCameraBall();
		
        // Draw a target on the paddle
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

// Draw the foreground level pieces...
void GameAssets::DrawFirstPassLevelPieces(double dT, const GameModel& gameModel, const Camera& camera) {
    
    Vector3D worldTransform = gameModel.GetCurrentLevelTranslation();
    BasicPointLight fgKeyLight, fgFillLight, ballLight;

    this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
    this->GetCurrentLevelMesh()->DrawFirstPassPieces(worldTransform, dT, camera, &gameModel, fgKeyLight, fgFillLight, ballLight);
}

void GameAssets::DrawFirstPassNoOutlinesLevelPieces(double dT, const Camera& camera) {
    this->GetCurrentLevelMesh()->DrawFirstPassNoOutlinesLevelPieces(dT, camera);
}

void GameAssets::DrawSecondPassLevelPieces(double dT, const GameModel& gameModel, const Camera& camera) {
    BasicPointLight fgKeyLight, fgFillLight, ballLight;

    this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);
    this->GetCurrentLevelMesh()->DrawSecondPassPieces(dT, camera, &gameModel, fgKeyLight, fgFillLight,
        ballLight, this->fboAssets->GetFullSceneFBO()->GetFBOTexture());
}

void GameAssets::DrawBoss(double dT, const Camera& camera, const GameModel& gameModel) {

    if (gameModel.GetCurrentLevel()->GetHasBoss()) {
        Vector2D levelTranslation = gameModel.GetCurrentLevelTranslation2D();

        glPushMatrix();
	    glTranslatef(levelTranslation[0], levelTranslation[1], 0);

	    BasicPointLight fgKeyLight, fgFillLight, ballLight;
	    this->lightAssets->GetBossAffectingLights(fgKeyLight, fgFillLight, ballLight);
        this->GetCurrentLevelMesh()->DrawBoss(dT, camera, gameModel, fgKeyLight, fgFillLight, ballLight, this);

        glPopMatrix();
    }
}

void GameAssets::DrawBossPostEffects(double dT, const Camera& camera, const GameModel& gameModel) {
    if (gameModel.GetCurrentLevel()->GetHasBoss()) {
        this->GetCurrentLevelMesh()->DrawBossPostEffects(dT, camera, gameModel);
    }
}

void GameAssets::DrawSafetyNetIfActive(double dT, const Camera& camera, const GameModel& gameModel) {

    if (gameModel.IsBottomSafetyNetActive() || this->bottomSafetyNetMesh->IsPlayingAnimation()) {
        this->DrawSafetyNet(this->bottomSafetyNetMesh, dT, camera, gameModel, 0.3f);
    }
    if (gameModel.IsTopSafetyNetActive() || this->topSafetyNetMesh->IsPlayingAnimation()) {
        this->DrawSafetyNet(this->topSafetyNetMesh, dT, camera, gameModel, -0.3f);
    }
}

void GameAssets::DrawSafetyNet(BallSafetyNetMesh* safetyNetMesh, double dT, const Camera& camera, 
                               const GameModel& gameModel, float yTranslationOffset) {
    
    safetyNetMesh->Tick(dT);

    // Deal with the special case of paddle camera mode: we don't want the safety net to not appear in paddle camera mode
    // and we want it to fade out/in when going into/out of paddle camera mode
    
    // NOTE: We also have to deal with the case of the safety net being where the paddle is
    // located (i.e., check to see if the paddle is flipped and whether we're dealing with the top or bottom
    // safety net before changing whether it's visible or not.

    const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
    if (paddle->GetIsPaddleCameraOn() || gameModel.GetTransformInfo()->GetIsPaddleCamAnimationActive() &&
        (safetyNetMesh == this->bottomSafetyNetMesh && !paddle->GetIsPaddleFlipped() || 
         safetyNetMesh == this->topSafetyNetMesh && paddle->GetIsPaddleFlipped()) ) {

        float paddleAlpha = paddle->GetAlpha();
        if (paddleAlpha <= 0.0f) {
            // Don't draw the safety net if the paddle is invisible and we're in paddle camera mode
            return;
        }
        else {
            safetyNetMesh->SetAlpha(paddleAlpha);
        }
    }
    else {
        safetyNetMesh->SetAlpha(1.0f);
    }

    BasicPointLight fgKeyLight, fgFillLight, ballLight;
    this->lightAssets->GetPieceAffectingLights(fgKeyLight, fgFillLight, ballLight);

    GameLevel* currLevel = gameModel.GetCurrentLevel();

    glPushMatrix();
    glTranslatef(-currLevel->GetLevelUnitWidth() / 2.0f, -(currLevel->GetLevelUnitHeight() / 2.0f + yTranslationOffset) + safetyNetMesh->GetMidYCoord(*currLevel), 0.0f);
    safetyNetMesh->Draw(camera, fgKeyLight, fgFillLight, ballLight);
    glPopMatrix();
}

void GameAssets::SetActiveEffectLights(const GameModel& gameModel, GameItem::ItemType itemEffectType) {
    switch (itemEffectType) {
        case GameItem::BlackoutItem:
            this->lightAssets->ToggleLights(false, GameLightAssets::DEFAULT_LIGHT_TOGGLE_TIME);
            break;

        case GameItem::PoisonPaddleItem: {
            this->lightAssets->StartStrobeLight(GameLightAssets::FGKeyLight, GameViewConstants::GetInstance()->POISON_LIGHT_LIGHT_COLOUR, 1.0f);
            this->lightAssets->StartStrobeLight(GameLightAssets::FGFillLight, GameViewConstants::GetInstance()->POISON_LIGHT_DEEP_COLOUR, 1.0f);
            this->lightAssets->StartStrobeLight(GameLightAssets::BallKeyLight, GameViewConstants::GetInstance()->POISON_LIGHT_LIGHT_COLOUR, 1.0f);
            break;
        }

        case GameItem::PaddleCamItem: {
            // Move the key light in the foreground so that it is behind the camera when it goes into paddle cam mode.
            float halfLevelHeight = gameModel.GetCurrentLevel()->GetLevelUnitHeight() / 2.0f;
            float halfLevelWidth  = gameModel.GetCurrentLevel()->GetLevelUnitWidth() / 2.0f;

            const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
            const Point2D& paddlePos = paddle->GetCenterPosition();

            Point3D newFGKeyLightPos(paddlePos[0], paddlePos[1] + PaddleCamHUD::PADDLE_CAM_FG_KEY_LIGHT_Y_OFFSET, 0.0f);
            Point3D newFGFillLightPos(-halfLevelWidth, -(halfLevelHeight + 10.0f), -5.0f);

            Matrix4x4 gameXYZTransform = gameModel.GetTransformInfo()->GetGameXYZTransform();
            Vector3D gameTranslation   = gameModel.GetCurrentLevelTranslation();

            newFGKeyLightPos  += gameTranslation;
            newFGKeyLightPos   = gameXYZTransform * newFGKeyLightPos;
            newFGFillLightPos += gameTranslation;
            newFGFillLightPos  = gameXYZTransform * newFGFillLightPos;

            this->lightAssets->ChangeLightPositionAndAttenuation(GameLightAssets::FGKeyLight, newFGKeyLightPos, 0.08f, 1.5f);
            this->lightAssets->ChangeLightPositionAndAttenuation(GameLightAssets::FGFillLight, newFGFillLightPos, 0.12f, 1.5f);

            break;
        }

        case GameItem::BallCamItem: {
            GameBall* cameraBall = gameModel.GetBallChosenForBallCamera();

            // Change the position of the key light so that it is facing down with the ball
            float halfLevelHeight = gameModel.GetCurrentLevel()->GetLevelUnitHeight() / 2.0f;

            const Point2D& cameraBallPos = cameraBall->GetCenterPosition2D();
            Point3D newFGKeyLightPos(cameraBallPos[0], cameraBallPos[1], BallCamHUD::BALL_CAM_FG_KEY_LIGHT_Z_POS);
            Point3D newFGFillLightPos(0.0f, -halfLevelHeight, BallCamHUD::BALL_CAM_FG_FILL_LIGHT_Z_POS);

            Matrix4x4 gameXYZTransform = gameModel.GetTransformInfo()->GetGameXYZTransform();
            Vector3D gameTranslation   = gameModel.GetCurrentLevelTranslation();

            newFGKeyLightPos  += gameTranslation;
            newFGKeyLightPos   = gameXYZTransform * newFGKeyLightPos;
            newFGFillLightPos += gameTranslation;
            newFGFillLightPos  = gameXYZTransform * newFGFillLightPos;

            this->lightAssets->ChangeLightPositionAndAttenuation(GameLightAssets::FGKeyLight,  newFGKeyLightPos,  0.08f, 1.5f);
            this->lightAssets->ChangeLightPositionAndAttenuation(GameLightAssets::FGFillLight, newFGFillLightPos, 0.12f, 1.5f);

            break;
        }

        default:
            break;
    }
}

void GameAssets::RemoveAndRestoreFromBeamLights(float timeToRestore) {
    this->lightAssets->RestoreLightPositionAndAttenuation(GameLightAssets::BeamLightFGKey, timeToRestore);
    this->lightAssets->RestoreLightColour(GameLightAssets::BeamLightFGKey, timeToRestore);
    this->lightAssets->RestoreLightPositionAndAttenuation(GameLightAssets::BeamLightFGFill, timeToRestore);
    this->lightAssets->RestoreLightColour(GameLightAssets::BeamLightFGFill, timeToRestore);
    this->lightAssets->RestoreLightPositionAndAttenuation(GameLightAssets::BeamLightPaddleKey, timeToRestore);
    this->lightAssets->RestoreLightColour(GameLightAssets::BeamLightPaddleKey, timeToRestore);
    this->lightAssets->RestoreLightPositionAndAttenuation(GameLightAssets::BeamLightPaddleFill, timeToRestore);
    this->lightAssets->RestoreLightColour(GameLightAssets::BeamLightPaddleFill, timeToRestore);
    this->lightAssets->RestoreLightPositionAndAttenuation(GameLightAssets::BeamLightBossKey, timeToRestore);
    this->lightAssets->RestoreLightColour(GameLightAssets::BeamLightBossKey, timeToRestore);
    this->lightAssets->RestoreLightPositionAndAttenuation(GameLightAssets::BeamLightBossFill, timeToRestore);
    this->lightAssets->RestoreLightColour(GameLightAssets::BeamLightBossFill, timeToRestore);
}

void GameAssets::DrawMiscEffects(const GameModel& gameModel) {
    // If the player is in RC rocket mode, then we draw the level boundaries for where the rocket can't be...
    if (gameModel.GetTransformInfo()->GetIsRemoteControlRocketCameraOn()) {
        
        Collision::AABB2D boundaries = gameModel.GenerateLevelProjectileBoundaries();
        const Point2D& min = boundaries.GetMin();
        const Point2D& max = boundaries.GetMax();

        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

        static const float BOUND_WIDTH = 5.0f;
        static const float BOUND_ALPHA = 0.5f;
        glBegin(GL_QUADS);
        
        // Left bound
        glColor4f(1.0f, 0.0f, 0.0f, BOUND_ALPHA);
        glVertex2f(min[0], min[1]);
        glVertex2f(min[0], max[1]);
        glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
        glVertex2f(min[0] - BOUND_WIDTH, max[1] + BOUND_WIDTH);
        glVertex2f(min[0] - BOUND_WIDTH, min[1] - BOUND_WIDTH);

        // Top bound
        glColor4f(1.0f, 0.0f, 0.0f, BOUND_ALPHA);
        glVertex2f(min[0], max[1]);
        glVertex2f(max[0], max[1]);
        glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
        glVertex2f(max[0] + BOUND_WIDTH, max[1] + BOUND_WIDTH);
        glVertex2f(min[0] - BOUND_WIDTH, max[1] + BOUND_WIDTH);

        // Right bound
        glColor4f(1.0f, 0.0f, 0.0f, BOUND_ALPHA);
        glVertex2f(max[0], max[1]);
        glVertex2f(max[0], min[1]);
        glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
        glVertex2f(max[0] + BOUND_WIDTH, min[1] - BOUND_WIDTH);
        glVertex2f(max[0] + BOUND_WIDTH, max[1] + BOUND_WIDTH);
        
        // Bottom bound
        glColor4f(1.0f, 0.0f, 0.0f, BOUND_ALPHA);
        glVertex2f(max[0], min[1]);
        glVertex2f(min[0], min[1]);
        glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
        glVertex2f(min[0] - BOUND_WIDTH, min[1] - BOUND_WIDTH);
        glVertex2f(max[0] + BOUND_WIDTH, min[1] - BOUND_WIDTH);

        glEnd();

        glPopAttrib();
    }
}

void GameAssets::DrawBeams(double dT, const GameModel& gameModel, const Camera& camera, 
                           const Vector2D& worldT) {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(camera);

	const std::list<Beam*>& beams = gameModel.GetActiveBeams();
	if (beams.empty()) {
		return;
	}

    static const float BRIGHT_BEAM_CENTER_MULTIPLER = 2.28f;
    static const float TYPICAL_INNER_BEAM_ALPHA = 0.8f;
	static const float TYPICAL_OUTER_BEAM_ALPHA = 0.4f;
	const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
	float innerBeamDepth;

	glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	glTranslatef(0, 0, 0);

	glBegin(GL_QUADS);

	Point3D temp;
	Point3D beamSegStart, beamSegEnd;
	Vector3D beamRightVec;
	Vector3D beamUpVec;
	Vector3D beamDepthVec;
	float currRadius, currAlpha, currZOffset;
	Vector2D tempRadiusOffset;
    bool paddleLaserBeamActive = false;

    int numBeamPositions = 0;
    Point2D avgBeamPos(0,0);
    Colour avgBeamColour(0,0,0);
    float avgBeamZOffset = PlayerPaddle::PADDLE_HALF_DEPTH;
    float avgRadiusFraction = 0.0f;

	for (std::list<Beam*>::const_iterator beamIter = beams.begin(); beamIter != beams.end(); ++beamIter) {
		const Beam* currentBeam = *beamIter;
        currAlpha = currentBeam->GetBeamAlpha();

        // Special cases for paddle laser beams -- if the paddle has been removed from the game then
        // we don't want to draw its beams either
        switch (currentBeam->GetType()) {
            case Beam::PaddleBeam: {
                paddleLaserBeamActive = currAlpha > 0;
                currZOffset = 0.0f;
                innerBeamDepth = paddle->GetHalfDepthTotal() / 2.0f;
                if (paddle->GetIsPaddleCameraOn()) {
                    currAlpha *= 0.25f;
                }
                break;
            }
            case Beam::BossBeam:
                innerBeamDepth = 0.01f;
                currZOffset = static_cast<const BossLaserBeam*>(currentBeam)->GetZOffset();
                break;

            default:
                assert(false);
                continue;
        }

        if (currAlpha <= 0) {
            continue;
        }

        avgBeamColour += currAlpha*currentBeam->GetBeamColour();
        avgBeamZOffset += currZOffset;
        
        const Colour& beamColour = currentBeam->GetBeamColour();
        Colour beamCenterColour  = BRIGHT_BEAM_CENTER_MULTIPLER * beamColour;
        
		const std::list<BeamSegment*>& beamSegments = currentBeam->GetBeamParts();
		std::list<BeamSegment*>::const_iterator segmentIter = beamSegments.begin();
		
		int segCounter = 0;
		const int NUM_BASE_SEGMENTS = currentBeam->GetNumBaseBeamSegments();

		for (; segmentIter != beamSegments.end(); ++segmentIter, ++segCounter) {
            const BeamSegment* currentSeg = *segmentIter;

            avgRadiusFraction += currentSeg->GetCurrentRadiusFraction();
            avgBeamPos.AddPoint(currentSeg->GetBeamSegmentRay().GetOrigin() + 
                LevelPiece::PIECE_HEIGHT*currentSeg->GetBeamSegmentRay().GetUnitDirection());
            numBeamPositions++;

            // Don't show the first few segments of the beam if it's a paddle beam and the
            // paddle camera item is active, otherwise it will fill the screen with beaminess
            // and just be annoying
            if (currentBeam->GetType() == Beam::PaddleBeam && 
                paddle->GetIsPaddleCameraOn() && segCounter < NUM_BASE_SEGMENTS) {
				continue;
			}
			beamSegStart = Point3D(currentSeg->GetStartPoint(), currZOffset);
			beamSegEnd   = Point3D(currentSeg->GetEndPoint(), currZOffset);
			beamUpVec    = Vector3D(currentSeg->GetBeamSegmentRay().GetUnitDirection());

			// Center of the beam (brighter in the center) 
			currRadius   = 0.5f * currentSeg->GetRadius();
			beamRightVec = currRadius * Vector3D(beamUpVec[1], -beamUpVec[0], 0);
			beamDepthVec = Vector3D(0, 0, 0.5f * innerBeamDepth);

			glColor4f(beamCenterColour.R(), beamCenterColour.G(), beamCenterColour.B(), TYPICAL_INNER_BEAM_ALPHA*currAlpha);
			
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
			beamDepthVec = Vector3D(0, 0, innerBeamDepth);

			glColor4f(beamColour.R(), beamColour.G(), beamColour.B(), TYPICAL_OUTER_BEAM_ALPHA*currAlpha);

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
		
            currZOffset = 0;
        }
	}
	glEnd();
	glPopMatrix();
	glPopAttrib();

    if (numBeamPositions > 0 && avgRadiusFraction > 0 && !gameModel.IsRemoteControlRocketActive()) {

        avgBeamPos        /= static_cast<float>(numBeamPositions);
        avgRadiusFraction /= static_cast<float>(numBeamPositions);
        avgBeamColour     /= static_cast<float>(beams.size());
        avgBeamZOffset    /= static_cast<float>(beams.size());

        // Grab a transform matrix from the game model to say where the light is
        // if the level is flipped or some such thing
        avgBeamPos += worldT;
        avgBeamPos = gameModel.GetTransformInfo()->GetGameXYZTransform() * avgBeamPos;

        static const float ATTENUATION_MULTIPLIER = 1.2f;
        float invAvgRadiusFract = (1.0f / avgRadiusFraction);
        
        static double timeCounter = 0;
        timeCounter += 7*dT + 5 * Randomizer::GetInstance()->RandomNumZeroToOne() * dT;
        Colour keyDiffuseColour = avgRadiusFraction*NumberFuncs::LerpOverFloat(-1.0, 1.0, avgBeamColour, 1.33f*(avgBeamColour + Colour(0.33,0.33,0.33)), sinf(timeCounter));
        Colour fillDiffuseColour = 0.5f*avgRadiusFraction*avgBeamColour;

        PointLight& beamLightFGKey = this->lightAssets->GetBeamLightFGKey();
        beamLightFGKey.SetLightOn(true, 0.0);
        beamLightFGKey.SetDiffuseColour(keyDiffuseColour);
        beamLightFGKey.SetPosition(Point3D(avgBeamPos, avgBeamZOffset));
        beamLightFGKey.SetLinearAttenuation(ATTENUATION_MULTIPLIER*GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_ATTEN*invAvgRadiusFract);

        PointLight& beamLightFGFill = this->lightAssets->GetBeamLightFGFill();
        beamLightFGFill.SetLightOn(true, 0.0);
        beamLightFGFill.SetDiffuseColour(fillDiffuseColour);
        beamLightFGFill.SetPosition(Point3D(avgBeamPos, GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_POSITION[2]));
        beamLightFGFill.SetLinearAttenuation(ATTENUATION_MULTIPLIER*GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_ATTEN*invAvgRadiusFract);

        PointLight& beamLightPaddleKey = this->lightAssets->GetBeamLightPaddleKey();
        beamLightPaddleKey.SetLightOn(true, 0.0);
        beamLightPaddleKey.SetDiffuseColour(keyDiffuseColour);
        beamLightPaddleKey.SetPosition(Point3D(avgBeamPos, avgBeamZOffset));
        beamLightPaddleKey.SetLinearAttenuation(ATTENUATION_MULTIPLIER*GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_ATTEN*invAvgRadiusFract);

        PointLight& beamLightPaddleFill = this->lightAssets->GetBeamLightPaddleFill();
        beamLightPaddleFill.SetLightOn(true, 0.0);
        beamLightPaddleFill.SetDiffuseColour(fillDiffuseColour);
        beamLightPaddleFill.SetPosition(Point3D(avgBeamPos, GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_POSITION[2]));
        beamLightPaddleFill.SetLinearAttenuation(ATTENUATION_MULTIPLIER*GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_ATTEN*invAvgRadiusFract);

        PointLight& beamLightBossKey = this->lightAssets->GetBeamLightBossKey();
        beamLightBossKey.SetLightOn(true, 0.0);
        beamLightBossKey.SetDiffuseColour(keyDiffuseColour);
        beamLightBossKey.SetPosition(Point3D(avgBeamPos, avgBeamZOffset));
        beamLightBossKey.SetLinearAttenuation(ATTENUATION_MULTIPLIER*GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_ATTEN*invAvgRadiusFract);

        PointLight& beamLightBossFill = this->lightAssets->GetBeamLightBossFill();
        beamLightBossFill.SetLightOn(true, 0.0);
        beamLightBossFill.SetDiffuseColour(fillDiffuseColour);
        beamLightBossFill.SetPosition(Point3D(avgBeamPos, GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_POSITION[2]));
        beamLightBossFill.SetLinearAttenuation(ATTENUATION_MULTIPLIER*GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_ATTEN*invAvgRadiusFract);

    }
    else {
        // Lights are moved back into their readjusted positions when a beam is removed from the game,
        // See function: RemoveBeamEffects
    }

    // If in paddle cam mode draw a full screen quad with the laser colour
    if (paddle->GetIsPaddleCameraOn() && paddleLaserBeamActive) {

        glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GeometryMaker::GetInstance()->DrawFullScreenQuadNoDepth(Camera::GetWindowWidth(), Camera::GetWindowHeight(), 
           ColourRGBA(GameModelConstants::GetInstance()->PADDLE_LASER_BEAM_COLOUR, 0.2f));

        glPopAttrib();
    }

	debug_opengl_state();
}

void GameAssets::DrawMeshProjectiles(double dT, const GameModel& gameModel, const Camera& camera) {
    // Get lights affecting the foreground meshes...
    BasicPointLight keyLight, fillLight, ballLight;
    this->lightAssets->GetPieceAffectingLights(keyLight, fillLight, ballLight);

    // Draw any rockets that are currently active...
    this->rocketMesh->Draw(dT, *gameModel.GetPlayerPaddle(), camera, keyLight, fillLight, ballLight, 
        this->fboAssets->GetFullSceneFBO()->GetFBOTexture());

    // Draw any mines that are currently active...
    this->mineMeshMgr->Draw(dT, camera, keyLight, fillLight, ballLight,
        this->fboAssets->GetFullSceneFBO()->GetFBOTexture());
}

/**
 * Draw informational game elements (e.g., tutorial bubbles, information about items acquired, etc.).
 */
void GameAssets::DrawInformativeGameElements(const Camera& camera, double dT, const GameModel& gameModel) {
    this->DrawRandomToItemAnimation(camera, dT, gameModel);
    this->DrawSkipDeathAnimation(camera, dT);
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
		this->crosshairHUD->Draw(paddle, displayWidth, displayHeight, (1.0 - paddle->GetAlpha()));
	}


	// Draw any pain overlay when active
	this->painHUD->Draw(dT, displayWidth, displayHeight);
	// Draw any flashes / fades
	this->flashHUD->Draw(dT, displayWidth, displayHeight);
}

const Point2D& GameAssets::GetBallSafetyNetPosition(bool bottomSafetyNet) const {
    if (bottomSafetyNet) {
        return this->bottomSafetyNetMesh->GetSafetyNetCenterPosition();
    }
    else {
        return this->topSafetyNetMesh->GetSafetyNetCenterPosition();
    }
}

/**
 * Call when the ball safety net has been created. This will prompt any animations /
 * effects associated with the mesh representing the safety net.
 */
void GameAssets::BallSafetyNetCreated(bool bottomSafetyNet) {
    if (bottomSafetyNet) {
	    this->bottomSafetyNetMesh->CreateBallSafetyNet();
    }
    else {
        this->topSafetyNetMesh->CreateBallSafetyNet();
    }
}

/**
 * Call when the ball safety net has been destroyed this will prompt any
 * animations / effects associated with the mesh representing the safety net.
 */
void GameAssets::BallSafetyNetDestroyed(const GameLevel& currLevel, const Point2D& pos, bool bottomSafetyNet) {
    if (bottomSafetyNet) {
	    this->bottomSafetyNetMesh->DestroyBallSafetyNet(currLevel, pos[0]);
    }
    else {
        this->topSafetyNetMesh->DestroyBallSafetyNet(currLevel, pos[0]);
    }
}

void GameAssets::LoadNewLevelMesh(const GameLevel& currLevel) {
	if (this->currentLevelMesh != NULL) {
		// If the level mesh object is already loaded then use it's proper method - this
		// avoids reloading a whole bunch of common assets
		this->currentLevelMesh->LoadNewLevel(this->sound, *this->worldAssets, *this->itemAssets, currLevel);
	}
	else {
		this->currentLevelMesh = new LevelMesh(this->sound, *this->worldAssets, *this->itemAssets, currLevel);
	}
    
    // Setup the ball safety net for the new level...
	this->bottomSafetyNetMesh->Regenerate(currLevel);
    this->topSafetyNetMesh->Regenerate(currLevel);

    // Load the lighting setup for the current level...
    this->worldAssets->LoadLightingForLevel(this, currLevel);
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
		this->paddleBeamAttachment = new PaddleBeamAttachment();
	}
    if (this->paddleMineAttachment == NULL) {
        this->paddleMineAttachment = new PaddleMineLauncher();
    }
	if (this->paddleGunAttachment == NULL) {
		this->paddleGunAttachment = new PaddleGunAttachment();
	}
	if (this->paddleStickyAttachment == NULL) {
		this->paddleStickyAttachment = new StickyPaddleGoo();
	}
    if (this->bottomSafetyNetMesh == NULL) {
        this->bottomSafetyNetMesh = new BallSafetyNetMesh(true);
    }
    if (this->topSafetyNetMesh == NULL) {
        this->topSafetyNetMesh = new BallSafetyNetMesh(false);
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

    if (this->paddleStatusEffectRenderer == NULL) {
        this->paddleStatusEffectRenderer = new PaddleStatusEffectRenderer(this->espAssets, this->sound);
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

        case Projectile::PaddleLaserBulletProjectile: {
            const LaserBulletProjectile& laserBullet = static_cast<const LaserBulletProjectile&>(projectile);
            if (!laserBullet.GetWasCreatedByReflectionOrRefraction()) {

                const PlayerPaddle& paddle = *gameModel.GetPlayerPaddle();

                // Add a sound for firing the laser
                this->sound->PlaySoundAtPosition(GameSound::LaserBulletShotEvent, false, paddle.GetPosition3D(), true, true, true);
                // Make the laser attachment animate for firing the gun...
                this->paddleGunAttachment->FirePaddleGun(paddle);
            }

			break;
        }

        case Projectile::PaddleFlameBlastProjectile: {
            const PlayerPaddle& paddle = *gameModel.GetPlayerPaddle();

            // Add a sound for firing the flame blast
            this->sound->PlaySoundAtPosition(GameSound::FlameBlasterShotEvent, false, paddle.GetPosition3D(), true, true, true);
            // Make the laser attachment animate for firing the gun...
            this->paddleGunAttachment->FirePaddleGun(paddle);

            break;
        }

        case Projectile::PaddleIceBlastProjectile: {
            const PlayerPaddle& paddle = *gameModel.GetPlayerPaddle();

            // Add a sound for firing the flame blast
            this->sound->PlaySoundAtPosition(GameSound::IceBlasterShotEvent, false, paddle.GetPosition3D(), true, true, true);
            // Make the laser attachment animate for firing the gun...
            this->paddleGunAttachment->FirePaddleGun(paddle);

            break;
        }

        case Projectile::BossLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
        case Projectile::BallLaserBulletProjectile: {

            const LaserBulletProjectile& laserBullet = static_cast<const LaserBulletProjectile&>(projectile);
            if (!laserBullet.GetWasCreatedByReflectionOrRefraction()) {
                this->sound->PlaySoundAtPosition(GameSound::LaserBulletShotEvent, false, 
                    projectile.GetPosition3D(), true, true, true, GameSound::DEFAULT_MIN_3D_SOUND_DIST, 0.5f);
            }
            break;
        }

        case Projectile::BossOrbBulletProjectile:
            this->sound->PlaySoundAtPosition(GameSound::GothicBossOrbAttackEvent, false,
                projectile.GetPosition3D(), true, true, true, GameSound::DEFAULT_MIN_3D_SOUND_DIST, 0.8f);
            break;

        case Projectile::BossLightningBoltBulletProjectile:
            this->sound->PlaySoundAtPosition(GameSound::DecoBossLightningBoltAttackEvent, false,
                projectile.GetPosition3D(), true, true, true);
            break;

        case Projectile::BossShockOrbBulletProjectile:
            this->sound->PlaySoundAtPosition(GameSound::DecoBossShockOrbAttackEvent, false,
                projectile.GetPosition3D(), true, true, true);
            break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            assert(dynamic_cast<const RocketProjectile*>(&projectile) != NULL);
			// Notify assets of the rocket...
			this->FireRocket(gameModel, *static_cast<const RocketProjectile*>(&projectile));
			break;

        case Projectile::PaddleRemoteCtrlRocketBulletProjectile: {
            assert(dynamic_cast<const PaddleRemoteControlRocketProjectile*>(&projectile) != NULL);
           
            const PaddleRemoteControlRocketProjectile* remoteCtrlRocket = static_cast<const PaddleRemoteControlRocketProjectile*>(&projectile);

            // Activate the remote control rocket HUD
            this->remoteControlRocketHUD->Activate(remoteCtrlRocket);

            // Notify assets of the rocket...
            this->FireRocket(gameModel, *remoteCtrlRocket);
            this->currentLevelMesh->UpdateNoEntryBlock(true);
            this->RemoveAndRestoreFromBeamLights(0.5);
            break;
        }

        case Projectile::PaddleMineBulletProjectile: {
            // Play a sound for the mine firing from the paddle
            sound->PlaySoundAtPosition(GameSound::PaddleMineFiredEvent, false, projectile.GetPosition3D(), true, true, true);

            // Notify the mine manager...
            const MineProjectile* mine = static_cast<const MineProjectile*>(&projectile);
            this->mineMeshMgr->AddMineProjectile(mine);
            this->paddleMineAttachment->FireMine();
            break;
        }
            
        case Projectile::MineTurretBulletProjectile: {
            // Play a sound for the mine firing from the turret
            sound->PlaySoundAtPosition(GameSound::TurretMineFiredEvent, false, projectile.GetPosition3D(), true, true, true);

            // Notify the mine manager...
            const MineProjectile* mine = static_cast<const MineProjectile*>(&projectile);
            this->mineMeshMgr->AddMineProjectile(mine);
            break;
        }

        case Projectile::PortalBlobProjectile: {
            // Play a sound for the portal being summoned into existence
            sound->PlaySoundAtPosition(GameSound::PortalProjectileOpenedEvent, false, projectile.GetPosition3D(), true, true, true, 50.0f);
            break;
        }

		default:
			break;
	}
}

/** 
 * Remove the given projectile and its effects from the assets.
 */
void GameAssets::RemoveProjectile(const GameModel& gameModel, const Projectile& projectile) {
	this->espAssets->RemoveProjectileEffect(projectile);
    
    double soundFadeoutTime = 0.0;

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
            this->currentLevelMesh->UpdateNoEntryBlock(false);
            break;

        case Projectile::FireGlobProjectile:
            this->espAssets->AddFireGlobDestroyedEffect(gameModel, projectile);
            break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile: {
            // Notify the mine manager...
            const MineProjectile* mine = static_cast<const MineProjectile*>(&projectile);
            this->mineMeshMgr->RemoveMineProjectile(mine);
            break;
        }

        case Projectile::CollateralBlockProjectile: {
            if (GameState::IsGameInPlayState(gameModel) && 
                gameModel.IsOutOfGameBoundsForProjectile(projectile.GetPosition())) {

                soundFadeoutTime = 1.0;
            }
            break;
        }

        case Projectile::PortalBlobProjectile: {
            // TODO?
            break;
        }

		default:
			break;
	}

    // Kill all sounds for the projectile
    this->sound->DetachAndStopAllSounds(&projectile, soundFadeoutTime);
}

/**
 * Private helper, causes the rocket to be fired and drawn.
 */
void GameAssets::FireRocket(const GameModel& gameModel, const RocketProjectile& rocketProjectile) {
    
    // Play a sound for a rocket firing and traveling (based off the type)
    switch (rocketProjectile.GetType()) {

        case Projectile::BossRocketBulletProjectile:
            // The rocket shot sound
            this->sound->PlaySoundAtPosition(GameSound::BossRocketFiredEvent, false, 
                rocketProjectile.GetPosition3D(), true, true, true);
            // Rocket moving sound loop
            this->sound->AttachAndPlaySound(&rocketProjectile, GameSound::BossRocketMovingLoop, 
                true, gameModel.GetCurrentLevelTranslation());
            break;
        
        case Projectile::RocketTurretBulletProjectile:
            // The rocket shot sound
            this->sound->PlaySoundAtPosition(GameSound::TurretRocketFiredEvent, false, rocketProjectile.GetPosition3D(), 
                true, true, true, GameSound::DEFAULT_MIN_3D_SOUND_DIST, 0.5f);
            // Rocket moving sound loop
            this->sound->AttachAndPlaySound(&rocketProjectile, GameSound::TurretRocketMovingLoop, 
                true, gameModel.GetCurrentLevelTranslation(), 0.5f);
            break;

        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::PaddleRocketBulletProjectile: {
            // The rocket launch sound
            this->sound->PlaySoundAtPosition(GameSound::PaddleRocketLaunchEvent, false, 
                rocketProjectile.GetPosition3D(), true, true, true);
            // Attach a moving sound loop to the rocket
            this->sound->AttachAndPlaySound(&rocketProjectile, GameSound::PaddleRocketMovingLoop, 
                true, gameModel.GetCurrentLevelTranslation());
            break;
        }

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
void GameAssets::PaddleHurtByProjectile(const PlayerPaddle& paddle, const Projectile& projectile, Camera& camera) {

	// Add the sprite/particle effect
	this->espAssets->AddPaddleHitByProjectileEffect(paddle, projectile, this->sound);

	switch (projectile.GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossLightningBoltBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
            this->sound->PlaySoundAtPosition(GameSound::PaddleLaserBulletCollisionEvent, false, 
                projectile.GetPosition3D(), true, true, true, 20.0f);
			break;

        case Projectile::RocketTurretBulletProjectile:
        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::BossShockOrbBulletProjectile:
            break;

        case Projectile::CollateralBlockProjectile:
            // Not necessary since it's already played when the collateral block is destroyed
            //this->sound->PlaySoundAtPosition(GameSound::PaddleCollateralBlockCollisionEvent, false, 
            //    projectile.GetPosition3D(), true, true, true, 10*PlayerPaddle::PADDLE_WIDTH_TOTAL);
            break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
			break;

		case Projectile::FireGlobProjectile: {

				const FireGlobProjectile* fireGlobProjectile = static_cast<const FireGlobProjectile*>(&projectile);
			    float multiplier = 1.0f / paddle.GetPaddleScaleFactor();
                //float globSizeMultiplier = static_cast<float>(fireGlobProjectile->GetRelativeSize()) / static_cast<float>(FireGlobProjectile::Large);
                
                switch (fireGlobProjectile->GetRelativeSize()) {
					case FireGlobProjectile::Small:
                        camera.ApplyCameraShake(multiplier*0.33, Vector3D(multiplier*1.5f*projectile.GetVelocityDirection(), multiplier*0.25f), 90);
                        GameControllerManager::GetInstance()->VibrateControllers(multiplier*0.33,
                            BBBGameController::SoftVibration, BBBGameController::VerySoftVibration);
						break;
					case FireGlobProjectile::Medium:
                        camera.ApplyCameraShake(multiplier*0.5, Vector3D(multiplier*1.75f*projectile.GetVelocityDirection(), multiplier*0.33f), 100);
                        GameControllerManager::GetInstance()->VibrateControllers(multiplier*0.5,
                            BBBGameController::MediumVibration, BBBGameController::SoftVibration);
						break;
					case FireGlobProjectile::Large:
                        camera.ApplyCameraShake(multiplier*0.8, Vector3D(multiplier*2.0f*projectile.GetVelocityDirection(), multiplier*0.5f), 110);
                        GameControllerManager::GetInstance()->VibrateControllers(multiplier*0.8,
                            BBBGameController::HeavyVibration, BBBGameController::MediumVibration);
						break;
					default:
						assert(false);
						break;
				}

                this->sound->PlaySoundAtPosition(GameSound::PaddleFireGlobCollisionEvent, false, 
                    projectile.GetPosition3D(), true, true, true, 20.0f);
			}
			break;

        case Projectile::PaddleFlameBlastProjectile: {
            float multiplier = static_cast<const PaddleFlameBlasterProjectile*>(&projectile)->GetSizeMultiplier();
            camera.ApplyCameraShake(multiplier * 1.0, Vector3D(1.5f*projectile.GetVelocityDirection(), 0.25f), 90);
            GameControllerManager::GetInstance()->VibrateControllers(multiplier * 0.33f,
                BBBGameController::MediumVibration, BBBGameController::MediumVibration);

            break;
        }

        case Projectile::PaddleIceBlastProjectile: {
            float multiplier = static_cast<const PaddleFlameBlasterProjectile*>(&projectile)->GetSizeMultiplier();
            camera.ApplyCameraShake(multiplier * 0.25, Vector3D(projectile.GetVelocityDirection(), 0.1f), 30);
            GameControllerManager::GetInstance()->VibrateControllers(multiplier * 0.25f,
                BBBGameController::SoftVibration, BBBGameController::SoftVibration);

            break;
        }

        case Projectile::PortalBlobProjectile:
            // Not implemented
            return;

		default:
			assert(false);
			return;
	}

	// Add a full screen overlay effect to show pain/badness
    PlayerHurtHUD::PainIntensity intensity = PlayerHurtHUD::GetIntensityForProjectile(projectile);
	this->painHUD->Activate(intensity);
}

void GameAssets::PaddleHurtByBeam(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment) {
    UNUSED_PARAMETER(beam);

    // Add the sprite/particle effects
    this->espAssets->AddPaddleHitByBeamEffect(paddle, beamSegment);

    // Add a full screen overlay effect to show pain/badness
    this->painHUD->Activate(PlayerHurtHUD::MajorPain);

    // Play sound
    this->sound->PlaySound(GameSound::PaddleLaserBeamCollisionEvent, false, true);

    // Vibrate the controller
    float intensityMultiplier = (beamSegment.GetRadius() / paddle.GetHalfWidthTotal());
    GameControllerManager::GetInstance()->VibrateControllers(intensityMultiplier * 1.25f,
        BBBGameController::HeavyVibration, BBBGameController::MediumVibration);
}

void GameAssets::PaddleHurtByBossBodyPart(const GameModel& gameModel, const PlayerPaddle& paddle, 
                                          const Boss& boss, const BossBodyPart& bossPart) {

    UNUSED_PARAMETER(gameModel);

    // Effects for the collision...
    this->espAssets->AddPaddleHitByBossPartEffect(paddle, bossPart);

    // Pain overlay and controller vibration
    this->painHUD->Activate(PlayerHurtHUD::MajorPain);
    GameControllerManager::GetInstance()->VibrateControllers(0.5,
        BBBGameController::HeavyVibration, BBBGameController::HeavyVibration);

    // Play the sound for the paddle being hurt -- this will depend on the type of boss
    switch (boss.GetWorldStyle()) {

        case GameWorld::Classical:
            this->sound->PlaySoundAtPosition(GameSound::ClassicalBossArmAttackHitEvent, false,
                paddle.GetPosition3D(), true, true, true);
            break;

        case GameWorld::Deco:
            this->sound->PlaySoundAtPosition(GameSound::DecoBossArmPaddleCollisionEvent, false,
                paddle.GetPosition3D(), true, true, true);
            break;

        default:
            break;
    }
}

// Notifies assets that there was an explosion and there should be a full-screen flash
void GameAssets::RocketExplosion(const RocketProjectile& rocket, Camera& camera, const GameModel* gameModel) {

    float forcePercentage = rocket.GetForcePercentageFactor();
    this->espAssets->AddRocketBlastEffect(forcePercentage, rocket.GetPosition());

    float flashMultiplier = 1.0f;
    if (gameModel->GetIsUnusualCameraModeActive()) {
        flashMultiplier *= 0.25f;
    }

	// Add a camera/controller shake and flash for when the rocket explodes...
    
    // Make boss rockets a bit weaker (interferes a bit less with the game play during the boss fight)
    if (rocket.GetType() == Projectile::BossRocketBulletProjectile) {
        camera.ApplyCameraShake(forcePercentage * 0.75f, forcePercentage * Vector3D(0.7f, 0.6f, 0.1f), 100);
        GameControllerManager::GetInstance()->VibrateControllers(forcePercentage * 1.2f,
            BBBGameController::SoftVibration, BBBGameController::MediumVibration);
        this->flashHUD->Activate(0.33, 0.75f * flashMultiplier);
    }
    else {
        camera.ApplyCameraShake(forcePercentage * 1.2f, forcePercentage * Vector3D(0.9f, 0.8f, 0.1f), 122);
        GameControllerManager::GetInstance()->VibrateControllers(forcePercentage * 1.33f,
            BBBGameController::MediumVibration, BBBGameController::MediumVibration);
        this->flashHUD->Activate(0.5, 1.0f * flashMultiplier);
    }

	// Play the explosion sound
	this->sound->PlaySoundAtPosition(GameSound::RocketExplodedEvent, false, rocket.GetPosition3D(), true, true, true);
}

void GameAssets::MineExplosion(const MineProjectile& mine, Camera& camera, const GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    this->espAssets->AddMineBlastEffect(mine, mine.GetPosition());

	// Add a camera/controller shake and flash for when the mine explodes...
    float forcePercentage = mine.GetVisualScaleFactor();
	camera.ApplyCameraShake(forcePercentage * 0.5f, forcePercentage * Vector3D(0.2f, 0.5f, 0.3f), 80);
    GameControllerManager::GetInstance()->VibrateControllers(forcePercentage * 0.75f,
        BBBGameController::VerySoftVibration, BBBGameController::SoftVibration);

	// Play the explosion sound
	this->sound->PlaySoundAtPosition(GameSound::MineExplodedEvent, false, mine.GetPosition3D(), true, true, true);
}

void GameAssets::FullscreenFlashExplosion(const FullscreenFlashEffectInfo& info, Camera& camera, const GameModel* gameModel) {
	
    float flashMultiplier = 1.0f;
    if (gameModel->GetIsUnusualCameraModeActive()) {
        flashMultiplier *= 0.25f;
    }

    // Add a camera/controller shake and fullscreen flash
    if (info.GetShakeMultiplier() > 0.0f) {
        double shakeTime = 0.9 * info.GetTime();
        camera.ApplyCameraShake(shakeTime, info.GetShakeMultiplier() * Vector3D(0.2f, 0.5f, 0.3f), 100);
        GameControllerManager::GetInstance()->VibrateControllers(shakeTime,
            BBBGameController::MediumVibration, BBBGameController::MediumVibration);
    }
    
    this->flashHUD->Activate(info.GetTime(), 1.0f * flashMultiplier);
}

void GameAssets::FullscreenFlash(double durationInS, float intensity) {
    this->flashHUD->Activate(durationInS, intensity);
}

void GameAssets::ToggleSkipLabel(bool activate) {
    if (activate) {
        this->skipLabel->Show(0.0, 0.5);
    }
    else {
        this->skipLabel->Unshow(0.0, 0.25);
    }
}

void GameAssets::ReinitializeSkipLabel() {
    this->skipLabel->Unshow(0.0, 0.0, true);
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
	LoadingScreen::GetInstance()->UpdateLoadingScreenWithRandomLoadStr();
	if (this->worldAssets == NULL || this->worldAssets->GetStyle() != world.GetStyle()) {
		// Delete all previously loaded style-related assets
		delete this->worldAssets;
		this->worldAssets = NULL;

		// Load up the new set of world geometry assets
		this->worldAssets = GameWorldAssets::CreateWorldAssets(world.GetStyle(), this);
		assert(this->worldAssets != NULL);
	}

	LoadingScreen::GetInstance()->UpdateLoadingScreenWithRandomLoadStr();
    this->ReinitializeAssets();
}

void GameAssets::ReinitializeAssets() {
	this->worldAssets->ResetToInitialState();

    this->paddleStatusEffectRenderer->Reinitialize();

	// Reinitialize the life HUD elements
	this->lifeHUD->Reinitialize();
    this->pointsHUD->Reinitialize();
    this->boostHUD->Reinitialize();
    this->ballReleaseHUD->Reinitialize();
    this->remoteControlRocketHUD->Reinitialize();
    this->ReinitializeSkipLabel();
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
            // Change the lighting in the level...
            this->SetActiveEffectLights(gameModel, GameItem::BlackoutItem);
			break;
        }
			
		case GameItem::PoisonPaddleItem: {
			// The poison item will make the lights turn a sickly green colour
			assert(gameModel.GetPlayerPaddle()->HasPaddleType(PlayerPaddle::PoisonPaddle));
            
            // Change the lighting in the level...
            this->SetActiveEffectLights(gameModel, GameItem::PoisonPaddleItem);

            // Apply the sound effect for poison...
            this->sound->ToggleSoundEffect(GameSound::PoisonEffect, true);
            break;
		}

		case GameItem::PaddleCamItem: {
			// For the paddle camera we remove the stars from all currently falling items (block the view of the player)
			// NOTE that this is not permanent and just does so for any currently falling items
			this->espAssets->TurnOffCurrentItemDropStars();
			
            // Change the lighting in the level...
            this->SetActiveEffectLights(gameModel, GameItem::PaddleCamItem);

			// Fade out the background...
			this->worldAssets->FadeBackground(true, 2.0f);
            break;
		}

		case GameItem::BallCamItem: {

            // Stop any sound associated with the cannon rotating if the ball that has the ball camera associated with it
            // is inside a cannon that was previously rotating
            GameBall* cameraBall = gameModel.GetBallChosenForBallCamera();
            if (cameraBall->IsLoadedInCannonBlock()) {
                const CannonBlock* cannon = cameraBall->GetCannonBlock();
                assert(cannon != NULL);
                this->sound->DetachAndStopSound(cannon, GameSound::CannonBlockRotatingLoop);
            }

			// For the paddle camera we remove the stars from all currently falling items (block the view of the player)
			// NOTE that this is not permanent and just does so for any currently falling items
			this->espAssets->TurnOffCurrentItemDropStars();

            // Change the lighting in the level...
            this->SetActiveEffectLights(gameModel, GameItem::BallCamItem);

			// Fade out the background...
			this->worldAssets->FadeBackground(true, 2.0f);
            break;
        }

		case GameItem::ShieldPaddleItem: {
            this->sound->StopAllSoundsWithType(GameSound::PaddleShieldDeactivatedEvent);
            this->sound->PlaySoundAtPosition(GameSound::PaddleShieldActivatedEvent, false, 
                gameModel.GetPlayerPaddle()->GetPosition3D(), true, true, true);

            const Texture2D* fullscreenFBOTex = this->GetFBOAssets()->GetFullSceneFBO()->GetFBOTexture();
			assert(fullscreenFBOTex != NULL);
			this->paddleShield->ActivateShield(*gameModel.GetPlayerPaddle(), *fullscreenFBOTex);
            break;
        }

        case GameItem::MagnetPaddleItem:
            this->sound->AttachAndPlaySound(gameModel.GetPlayerPaddle(), GameSound::MagnetPaddleLoop, true,
                gameModel.GetCurrentLevelTranslation());
            this->magnetPaddleEffect->Reset();
            break;

        case GameItem::UpsideDownItem:
            // Moved to the GameTransformMgr
            //this->sound->PlaySound(GameSound::LevelFlipEvent, false);
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

        case GameItem::MineLauncherPaddleItem:
            this->paddleMineAttachment->Activate();
            break;

		default:
			break;
	}
}

/**
 * Deactivate the effect for a particular item - this will clean up anything that was activated
 * which needs manual clean up.
 */
void GameAssets::DeactivateItemEffects(const GameModel& gameModel, const GameItem& item, bool gameIsInPlay) {
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
            if (gameIsInPlay) {
                this->sound->PlaySoundAtPosition(GameSound::PaddleShieldDeactivatedEvent, false, 
                    gameModel.GetPlayerPaddle()->GetPosition3D(), true, true, true);
            }
			this->paddleShield->DeactivateShield();
            break;

        case GameItem::UpsideDownItem:
            // Moved to the GameTransformMgr
            //if (gameIsInPlay || gameModel.GetCurrentStateType() == GameState::BallDeathStateType) {
            //    this->sound->PlaySound(GameSound::LevelUnflipEvent, false);
            //}
            break;

        case GameItem::MagnetPaddleItem:
            this->sound->DetachAndStopSound(gameModel.GetPlayerPaddle(), GameSound::MagnetPaddleLoop);
            break;

        case GameItem::MineLauncherPaddleItem:
            this->paddleMineAttachment->Deactivate();
            break;

		default:
			break;
	}
}

void GameAssets::RemoveBeamEffects(const GameModel& gameModel, const Beam& beam) {
    UNUSED_PARAMETER(beam);

    // Check to see if there are still beams active, if not we need to restore the beam light...
    if (gameModel.GetActiveBeams().empty()) {

        // Restore the beam light to its normal position and colour...
        this->RemoveAndRestoreFromBeamLights(0.5);

        // Figure out what effects are currently active and make sure we maintain the lighting for those effects...
        const std::list<GameItemTimer*>& activeTimers = gameModel.GetActiveTimers();
        for (std::list<GameItemTimer*>::const_iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
            this->SetActiveEffectLights(gameModel, (*iter)->GetTimerItemType());
        }
    }
}

void GameAssets::ActivatePaddleStatusEffect(const GameModel& gameModel, Camera& camera, 
                                            PlayerPaddle::PaddleSpecialStatus status) {
    UNUSED_PARAMETER(camera);
    const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();

    switch(status) {

        case PlayerPaddle::FrozenInIceStatus: {
            this->paddleStatusEffectRenderer->ToggleFrozen(*paddle, true);
            this->espAssets->AddPaddleFrozeEffect(*paddle);
            this->flashHUD->Activate(0.25, 1.0f);
            break;
        }

        case PlayerPaddle::OnFireStatus: {
            this->paddleStatusEffectRenderer->ToggleOnFire(*paddle, true);
            break;
        }

        case PlayerPaddle::ElectrocutedStatus: {
            this->paddleStatusEffectRenderer->ToggleElectrocuted(*paddle, true);
            this->espAssets->AddPaddleElectrocutedEffect(*paddle);
            this->flashHUD->Activate(0.33, 1.0f);
            break;
        }

        default:
            break;
    }

}

void GameAssets::DeactivatePaddleStatusEffect(const GameModel& gameModel, Camera& camera, 
                                              PlayerPaddle::PaddleSpecialStatus status) {

    const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();

    switch(status) {

        case PlayerPaddle::FrozenInIceStatus: {
            this->paddleStatusEffectRenderer->ToggleFrozen(*paddle, false);
            this->espAssets->AddPaddleUnfrozeEffect(*paddle);

            float scaleFactor = paddle->GetPaddleScaleFactor();

            camera.ApplyCameraShake(scaleFactor * 0.5, Vector3D(0.25f, 0.9f, 0.1f), 50);
            GameControllerManager::GetInstance()->VibrateControllers(scaleFactor * 0.2,
                BBBGameController::SoftVibration, BBBGameController::SoftVibration);

            this->flashHUD->Activate(0.25, 0.75f);
            break;
        }

        case PlayerPaddle::OnFireStatus: {
            this->paddleStatusEffectRenderer->ToggleOnFire(*paddle, false);
            break;
        }

        case PlayerPaddle::ElectrocutedStatus: {
            this->paddleStatusEffectRenderer->ToggleElectrocuted(*paddle, false);
            break;
        }

        default:
            break;
    }
}

void GameAssets::CancelFrozenPaddleWithFireEffect(const PlayerPaddle& paddle) {

    Point3D paddlePos3d = paddle.GetPosition3D();

    this->sound->PlaySoundAtPosition(GameSound::IceMeltedEvent, false, 
        paddlePos3d, true, true, true);

    this->espAssets->AddIceMeltedByFireEffect(
        paddlePos3d, 2.0f*paddle.GetHalfWidthTotal(), 2.0f*paddle.GetHalfHeight());

    this->paddleStatusEffectRenderer->CancelFrozenWithOnFire();
}

void GameAssets::CancelOnFirePaddleWithIceEffect(const PlayerPaddle& paddle) {
    
    Point3D paddlePos3d = paddle.GetPosition3D();
    
    this->sound->PlaySoundAtPosition(GameSound::FireFrozeEvent, false, 
        paddlePos3d, true, true, true);

    this->espAssets->AddFirePutOutByIceEffect(
        paddlePos3d, 2.0f*paddle.GetHalfWidthTotal(), 2.0f*paddle.GetHalfHeight());

    this->paddleStatusEffectRenderer->CancelOnFireWithFrozen();
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

void GameAssets::DrawRandomToItemAnimation(const Camera& camera, double dT, const GameModel& gameModel) {
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
    Matrix4x4 gameTransform = gameModel.GetTransformInfo()->GetGameXYZTransform();
    glMultMatrixf(gameTransform.begin());
    glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], negHalfLevelDim[2]);

    // Draw the random item to actual item animation when required...
    this->randomToItemAnimation.Draw(camera, dT);

    glPopMatrix();

    glPopAttrib();
}

void GameAssets::DrawSkipDeathAnimation(const Camera& camera, double dT) {
    
    this->skipLabel->Tick(dT);
    if (this->skipLabel->GetAlpha() <= 0.0f) {
        return;
    }

    static const float HORIZONTAL_GAP = 10;
    static const float VERTICAL_GAP   = 10;

    this->skipLabel->SetTopLeftCorner(
        Camera::GetWindowWidth() - this->skipLabel->GetWidth() - HORIZONTAL_GAP, this->skipLabel->GetHeight() + VERTICAL_GAP);

    this->skipLabel->Draw(camera);
}