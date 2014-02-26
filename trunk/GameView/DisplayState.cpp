/**
 * DisplayState.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#include "DisplayState.h"
#include "GameDisplay.h"
#include "MainMenuDisplayState.h"
#include "SelectWorldMenuState.h"
#include "SelectLevelMenuState.h"
#include "BlammopediaState.h"
#include "LevelStartDisplayState.h"
#include "InTutorialGameDisplayState.h"
#include "InGameDisplayState.h"
#include "InGameBossLevelDisplayState.h"
#include "InGameMenuState.h"
#include "LevelEndDisplayState.h"
#include "LevelCompleteSummaryDisplayState.h"
#include "BossLevelCompleteSummaryDisplayState.h"
#include "GameCompleteDisplayState.h"
#include "GameOverDisplayState.h"
#include "CgFxSkybox.h"

#include "../GameModel/BallBoostModel.h"
#include "../GameModel/Beam.h"

DisplayState::DisplayState(GameDisplay* display) : display(display) {
}

DisplayState::~DisplayState() {
}

// Factory method for building a display state from a given enumerated
// type. Caller takes ownership of returned memory.
DisplayState* DisplayState::BuildDisplayStateFromType(const DisplayStateType& type, const DisplayStateInfo& info,
                                                      GameDisplay* display) {
	switch (type) {
		case DisplayState::MainMenu:
			return new MainMenuDisplayState(display, info);
        case DisplayState::SelectWorldMenu:
            return new SelectWorldMenuState(display, info);
        case DisplayState::SelectLevelMenu:
            return new SelectLevelMenuState(display, info);
        case DisplayState::BlammopediaMenu:
            return new BlammopediaState(display);
		case DisplayState::LevelStart:
			return new LevelStartDisplayState(display);
        case DisplayState::InTutorialGame:
            return new InTutorialGameDisplayState(display);
		case DisplayState::InGame:
			return new InGameDisplayState(display);
        case DisplayState::InGameBossLevel:
            return new InGameBossLevelDisplayState(display);
		case DisplayState::InGameMenu:
			return new InGameMenuState(display, NULL);
		case DisplayState::LevelEnd:
			return new LevelEndDisplayState(display);
        case DisplayState::LevelCompleteSummary:
            return new LevelCompleteSummaryDisplayState(display);
        case DisplayState::BossLevelCompleteSummary:
            return new BossLevelCompleteSummaryDisplayState(display);
		case DisplayState::GameComplete:
			return new GameCompleteDisplayState(display);
		case DisplayState::GameOver:
			return new GameOverDisplayState(display);

		default:
			assert(false);
			break;
	}

	return NULL;
}

#ifdef _DEBUG
/**
 * Debugging function that draws the collision boundaries of level pieces.
 */
void DisplayState::DebugDrawBounds() {
	if (!GameDisplay::IsDrawDebugBoundsOn()) { return; }
	Vector2D negHalfLevelDim = -0.5f * this->display->GetModel()->GetLevelUnitDimensions();

	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Draw boundary of paddle...
	GameModel* model = this->display->GetModel();
    model->GetPlayerPaddle()->DebugDraw();
	
    // Draw debug info for each ball...
    const std::list<GameBall*>& balls = model->GetGameBalls();
    for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {
        const GameBall* ball = *iter;
        ball->DebugDraw();
    }

	// Draw of boundaries of each block...
	const std::vector<std::vector<LevelPiece*>>& pieces = model->GetCurrentLevel()->GetCurrentLevelLayout();
	for (size_t i = 0; i < pieces.size(); i++) {
		const std::vector<LevelPiece*>& setOfPieces = pieces[i];
		for (size_t j = 0; j < setOfPieces.size(); j++) {
			setOfPieces[j]->DebugDraw();
		}
	}

	// Draw any beam rays...
	std::list<Beam*>& beams = model->GetActiveBeams();
	for (std::list<Beam*>::const_iterator iter = beams.begin(); iter != beams.end(); ++iter) {
		(*iter)->DebugDraw();
	}

    // Draw the debug bounds of all balls
    const BallBoostModel* boostModel = model->GetBallBoostModel();
    if (boostModel != NULL && boostModel->GetBulletTimeState() != BallBoostModel::NotInBulletTime) {
        boostModel->DebugDraw();
    }

    // Draw the debug bounds of any boss
    const GameLevel* currLevel = model->GetCurrentLevel();
    const Boss* boss = currLevel->GetBoss();
    if (boss != NULL) {
        boss->DebugDraw();
    }

    // Draw boundaries of all projectiles
    const GameModel::ProjectileMap& projectiles = model->GetActiveProjectiles();
    for (GameModel::ProjectileMapConstIter iter1 = projectiles.begin(); iter1 != projectiles.end(); ++iter1) {

        const GameModel::ProjectileList& projectileList = iter1->second; 
        for (GameModel::ProjectileListConstIter iter2 = projectileList.begin(); iter2 != projectileList.end(); ++iter2) {
            (*iter2)->DebugDraw();
        }
    }

	glPopMatrix();

	debug_opengl_state();
}
#endif
