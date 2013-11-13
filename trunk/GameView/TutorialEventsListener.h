/**
 * TutorialEventsListener.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __TUTORIALEVENTSLISTENER_H__
#define __TUTORIALEVENTSLISTENER_H__

#include "../GameModel/GameEvents.h"
#include "../GameControl/BBBGameController.h"

#include "../ESPEngine/ESP.h"

#include "ButtonTutorialHint.h"
#include "BasicMultiTutorialHint.h"
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
    void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle);
    void BallShotEvent(const GameBall& shotBall);
    void BlockDestroyedEvent(const LevelPiece& block, const LevelPiece::DestructionMethod& method);
    void ItemActivatedEvent(const GameItem& item);
    void PaddleWeaponFiredEvent();
    void LivesChangedEvent(int livesLeftBefore, int livesLeftAfter);
    void AllBallsDeadEvent(int livesLeft);
    void LastBallAboutToDieEvent(const GameBall& lastBallToDie);

    void BallBoostGainedEvent();
    void BallBoostLostEvent(bool allBoostsLost);
    void BallBoostUsedEvent();
    void BulletTimeStateChangedEvent(const BallBoostModel& boostModel);
    void NumStarsChangedEvent(const PointAward* pointAward, int oldNumStars, int newNumStars);

    // Set methods for various tutorial hints
    void SetMovePaddleHint(ButtonTutorialHint* hint) { this->movePaddleHint = hint; hint->Show(0.0, 0.75); }
    void SetShootBallHint(ButtonTutorialHint* hint)  { this->shootBallHint = hint; }
    void SetFireWeaponHint(ButtonTutorialHint* hint) { this->fireWeaponHint = hint; }
    void SetStartBoostHint(ButtonTutorialHint* hint) { this->startBoostHint = hint; }
    void SetDoBoostPressToReleaseHint(ButtonTutorialHint* hint)    { this->doBoostPressToReleaseHint = hint; }
    void SetDoBoostSlingshotHint(ButtonTutorialHint* hint)    { this->doBoostSlingshotHint = hint; }
    void SetHoldBoostHint(ButtonTutorialHint* hint)  { this->holdBoostHint = hint; }
    void SetBoostAvailableHint(ButtonTutorialHint* hint)  { this->boostAvailableHint = hint; }
    
    //void SetPointsHintEmitter(ESPPointEmitter* hint) { this->pointsTutorialHintEmitter = hint; }

    void SetMultiplierHints(BasicMultiTutorialHint* hints) { this->multiplierHints = hints; }
    void SetMultiplierLostHint(BasicMultiTutorialHint* hint) { this->multiplierLostHint = hint; }

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
    bool hasShownBoostHint;

    // Tutorial hints
    ButtonTutorialHint* movePaddleHint;
    ButtonTutorialHint* shootBallHint;
    ButtonTutorialHint* fireWeaponHint;
    ButtonTutorialHint* startBoostHint;
    ButtonTutorialHint* doBoostPressToReleaseHint;
    ButtonTutorialHint* doBoostSlingshotHint;
    ButtonTutorialHint* holdBoostHint;
    ButtonTutorialHint* boostAvailableHint;

    BasicMultiTutorialHint* multiplierLostHint;
    BasicMultiTutorialHint* multiplierHints;

    //ESPPointEmitter* pointsTutorialHintEmitter;
    //ESPParticleColourEffector fadeEffector;

    DISALLOW_COPY_AND_ASSIGN(TutorialEventsListener);
};

inline void TutorialEventsListener::BallShotEvent(const GameBall& shotBall) {
    UNUSED_PARAMETER(shotBall);
    this->shootBallHint->Unshow(0.0, 0.5);
}

#endif // __TUTORIALEVENTSLISTENER_H__