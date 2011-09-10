/**
 * TutorialEventsListener.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TUTORIALEVENTSLISTENER_H__
#define __TUTORIALEVENTSLISTENER_H__

#include "../GameModel/GameEvents.h"
#include "../GameControl/BBBGameController.h"
#include "../GameSound/GameSoundAssets.h"

#include "ButtonTutorialHint.h"

class GameDisplay;

class TutorialEventsListener : public GameEvents {
public:
    TutorialEventsListener();
    ~TutorialEventsListener();

    // Additional event methods
    void ButtonPressed(const GameControl::ActionButton& pressedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton);

    // Overridden event methods
    void BallShotEvent(const GameBall& shotBall);
    void BlockDestroyedEvent(const LevelPiece& block);

    // Set methods for various tutorial hints
    void SetMovePaddleHint(ButtonTutorialHint* hint) { this->movePaddleHint = hint; hint->Show(0.0, 1.0); }
    void SetShootHint(ButtonTutorialHint* hint) { this->shootHint = hint; }

private:
    // Basic values to keep track of part of the game state, used to determine
    // whether certain tutorial hints are shown or not
    int numBlocksDestroyed;

    // Flags for showing or not showing certain tutorial hints
    // int numTimesShootHintShown;
    bool movePaddleHintUnshown;

    // Tutorial hints
    ButtonTutorialHint* movePaddleHint;
    ButtonTutorialHint* shootHint;


    DISALLOW_COPY_AND_ASSIGN(TutorialEventsListener);
};

inline void TutorialEventsListener::BallShotEvent(const GameBall& shotBall) {
    UNUSED_PARAMETER(shotBall);
    this->shootHint->Unshow(0.0, 1.0);
}

inline void TutorialEventsListener::BlockDestroyedEvent(const LevelPiece& block) {
    UNUSED_PARAMETER(block);
    this->numBlocksDestroyed++;
}

#endif // __TUTORIALEVENTSLISTENER_H__