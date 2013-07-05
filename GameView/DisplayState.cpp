/**
 * DisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "DisplayState.h"
#include "GameDisplay.h"
#include "MainMenuDisplayState.h"
#include "SelectWorldMenuState.h"
#include "SelectLevelMenuState.h"
#include "BlammopediaState.h"
#include "LevelStartDisplayState.h"
#include "WorldStartDisplayState.h"
#include "InTutorialGameDisplayState.h"
#include "InGameDisplayState.h"
#include "InGameBossLevelDisplayState.h"
#include "InGameMenuState.h"
#include "LevelEndDisplayState.h"
#include "LevelCompleteSummaryDisplayState.h"
#include "BossLevelCompleteSummaryDisplayState.h"
#include "GameCompleteDisplayState.h"
#include "GameOverDisplayState.h"

#include "../GameModel/BallBoostModel.h"
#include "../GameModel/Beam.h"

// Factory method for building a display state from a given enumerated
// type. Caller takes ownership of returned memory.
DisplayState* DisplayState::BuildDisplayStateFromType(const DisplayStateType& type, GameDisplay* display) {
	switch (type) {
		case DisplayState::MainMenu:
			return new MainMenuDisplayState(display);
        case DisplayState::SelectWorldMenu:
            return new SelectWorldMenuState(display);

        case DisplayState::SelectLevelMenu:
            assert(false);
            return NULL;

        case DisplayState::BlammopediaMenu:
            return new BlammopediaState(display);
		case DisplayState::LevelStart:
			return new LevelStartDisplayState(display);
		case DisplayState::WorldStart:
			return new WorldStartDisplayState(display);
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
 * Debugging function that draws the collision boundries of level pieces.
 */
void DisplayState::DebugDrawBounds() {
	if (!GameDisplay::IsDrawDebugBoundsOn()) { return; }
	Vector2D negHalfLevelDim = -0.5f * this->display->GetModel()->GetLevelUnitDimensions();

	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Draw boundry of paddle...
	this->display->GetModel()->GetPlayerPaddle()->DebugDraw();
	
	// Draw of boundries of each block...
	std::vector<std::vector<LevelPiece*>> pieces = this->display->GetModel()->GetCurrentLevel()->GetCurrentLevelLayout();
	for (size_t i = 0; i < pieces.size(); i++) {
		std::vector<LevelPiece*> setOfPieces = pieces[i];
		for (size_t j = 0; j < setOfPieces.size(); j++) {
			setOfPieces[j]->DebugDraw();
		}
	}

	// Draw any beam rays...
	std::list<Beam*>& beams = this->display->GetModel()->GetActiveBeams();
	for (std::list<Beam*>::const_iterator iter = beams.begin(); iter != beams.end(); ++iter) {
		(*iter)->DebugDraw();
	}

    // Draw the debug bounds of all balls
    const BallBoostModel* boostModel = this->display->GetModel()->GetBallBoostModel();
    if (boostModel != NULL && boostModel->GetBulletTimeState() != BallBoostModel::NotInBulletTime) {
        boostModel->DebugDraw();
    }

    // Draw the debug bounds of any boss
    const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
    const Boss* boss = currLevel->GetBoss();
    if (boss != NULL) {
        boss->DebugDraw();
    }

    // Draw boundaries of all projectiles
    const std::list<Projectile*>& projectiles = this->display->GetModel()->GetActiveProjectiles();
    for (std::list<Projectile*>::const_iterator iter = projectiles.begin(); iter != projectiles.end(); ++iter) {
        (*iter)->DebugDraw();
    }

	glPopMatrix();

	debug_opengl_state();
}
#endif