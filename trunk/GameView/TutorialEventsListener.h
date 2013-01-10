/**
 * TutorialEventsListener.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2011
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

#include "../ESPEngine/ESP.h"

#include "ButtonTutorialHint.h"
#include "PopupTutorialHint.h"

class GameDisplay;

class TutorialEventsListener : public GameEvents {
public:
    TutorialEventsListener(GameDisplay* display);
    ~TutorialEventsListener();

    //void Tick(double dT);

    // Additional event methods
    void ButtonPressed(const GameControl::ActionButton& pressedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton);

    // Overridden event methods
    void BallShotEvent(const GameBall& shotBall);
    void BlockDestroyedEvent(const LevelPiece& block, const LevelPiece::DestructionMethod& method);
    void ItemActivatedEvent(const GameItem& item);
    void PaddleWeaponFiredEvent();
    void LivesChangedEvent(int livesLeftBefore, int livesLeftAfter);
    void AllBallsDeadEvent(int livesLeft);
    void LastBallAboutToDieEvent(const GameBall& lastBallToDie);

    void BallBoostGainedEvent();
    void BallBoostLostEvent(bool allBoostsLost);
    void BulletTimeStateChangedEvent(const BallBoostModel& boostModel);
    void NumStarsChangedEvent(int oldNumStars, int newNumStars);

    // Set methods for various tutorial hints
    void SetMovePaddleHint(ButtonTutorialHint* hint) { this->movePaddleHint = hint; hint->Show(0.0, 0.75); }
    void SetShootBallHint(ButtonTutorialHint* hint)  { this->shootBallHint = hint; }
    void SetFireWeaponHint(ButtonTutorialHint* hint) { this->fireWeaponHint = hint; }
    void SetStartBoostHint(ButtonTutorialHint* hint) { this->startBoostHint = hint; }
    void SetDoBoostPressToReleaseHint(ButtonTutorialHint* hint)    { this->doBoostPressToReleaseHint = hint; }
    void SetDoBoostSlingshotHint(ButtonTutorialHint* hint)    { this->doBoostSlingshotHint = hint; }
    void SetHoldBoostHint(ButtonTutorialHint* hint)  { this->holdBoostHint = hint; }
    void SetBoostAvailableHint(ButtonTutorialHint* hint)  { this->boostAvailableHint = hint; }
    
    void SetPointsHintEmitter(ESPPointEmitter* hint) { this->pointsTutorialHintEmitter = hint; }

private:
    GameDisplay* display;

    // Basic values to keep track of part of the game state, used to determine
    // whether certain tutorial hints are shown or not
    int numBlocksDestroyed;
    int numBallDeaths;

    // Flags for showing or not showing certain tutorial hints
    // int numTimesShootHintShown;
    bool movePaddleHintUnshown;
    bool fireWeaponAlreadyShown;
    bool keepShowingBoostHint;
    bool multPopupHintAlreadyShown;
    bool finishedPointsHint;

    // Tutorial hints
    ButtonTutorialHint* movePaddleHint;
    ButtonTutorialHint* shootBallHint;
    ButtonTutorialHint* fireWeaponHint;
    ButtonTutorialHint* startBoostHint;
    ButtonTutorialHint* doBoostPressToReleaseHint;
    ButtonTutorialHint* doBoostSlingshotHint;
    ButtonTutorialHint* holdBoostHint;
    ButtonTutorialHint* boostAvailableHint;

    ESPPointEmitter* pointsTutorialHintEmitter;
    ESPParticleColourEffector fadeEffector;

    DISALLOW_COPY_AND_ASSIGN(TutorialEventsListener);
};

inline void TutorialEventsListener::BallShotEvent(const GameBall& shotBall) {
    UNUSED_PARAMETER(shotBall);
    this->shootBallHint->Unshow(0.0, 0.5);
}

inline void TutorialEventsListener::PaddleWeaponFiredEvent() {
    this->fireWeaponHint->Unshow(0.0, 1.0, true);
}

#endif // __TUTORIALEVENTSLISTENER_H__