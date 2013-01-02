/**
 * Boss.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Boss.h"
#include "AbstractBossBodyPart.h"
#include "BossBodyPart.h"
#include "BossAIState.h"
#include "ClassicalBoss.h"

Boss::Boss() : currAIState(NULL), nextAIState(NULL), root(NULL)  {
}

Boss::~Boss() {
    // Clean up the body parts
    for (std::vector<AbstractBossBodyPart*>::iterator iter = this->bodyParts.begin();
         iter != this->bodyParts.end(); ++iter) {

        AbstractBossBodyPart* part = *iter;
        delete part;
        part = NULL;
    }
    this->bodyParts.clear();
    this->root = NULL;

	// Delete the state
    if (this->currAIState != NULL) {
	    delete this->currAIState;
	    this->currAIState = NULL;
    }
    if (this->nextAIState != NULL) {
        delete this->nextAIState;
        this->nextAIState = NULL;
    }
}

/**
 * Static factory function for building boss types based on a given world style.
 */
Boss* Boss::BuildStyleBoss(const GameWorld::WorldStyle& style) {
    Boss* boss = NULL;
    switch (style) {
        case GameWorld::Classical:
            boss = new ClassicalBoss();
            break;
            
        case GameWorld::Deco:
            // TODO

        case GameWorld::Futurism:
            // TODO

        default:
            assert(false);
            break;
    }
    if (boss != NULL) {
        boss->Init();
    }

    return boss;
}

void Boss::Tick(double dT, GameModel* gameModel) {
    // Tick the body parts of the boss
    this->root->Tick(dT, gameModel);
    
    // Tick the current state of the boss
    if (this->currAIState != NULL) {
        this->currAIState->Tick(dT, gameModel);
    }

    // Update the AI state if necessary
    this->UpdateAIState();
}

void Boss::SetNextAIState(BossAIState* nextState) {
    if (this->nextAIState != NULL) {
    	delete this->nextAIState;
    }
    this->nextAIState = nextState;
}

void Boss::UpdateAIState() {
	if (this->nextAIState != NULL) {
		this->SetCurrentAIStateImmediately(this->nextAIState);
		this->nextAIState = NULL;
	}
}

void Boss::SetCurrentAIStateImmediately(BossAIState* newState) {
	assert(newState != NULL);
	if (this->currAIState != NULL) {
		delete this->currAIState;
	}
	this->currAIState = newState;
}

#ifdef _DEBUG
void Boss::DebugDraw() const {
    if (this->root != NULL) {
        this->root->DebugDraw();
    }
}
#endif