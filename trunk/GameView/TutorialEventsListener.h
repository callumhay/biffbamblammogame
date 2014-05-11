/**
 * TutorialEventsListener.h
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
    void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle, bool hitPaddleUnderside);
    void BallShotEvent(const GameBall& shotBall);
    void BlockDestroyedEvent(const LevelPiece& block, const LevelPiece::DestructionMethod& method);
    void ItemActivatedEvent(const GameItem& item);
    void PaddleWeaponFiredEvent(PlayerPaddle::PaddleType weaponType);
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
    bool hasShownMultiplierHints;
    bool hasShownMultiplierLostHint;

    DISALLOW_COPY_AND_ASSIGN(TutorialEventsListener);
};

inline void TutorialEventsListener::BallShotEvent(const GameBall& shotBall) {
    UNUSED_PARAMETER(shotBall);
    this->shootBallHint->Unshow(0.0, 0.5);
}

#endif // __TUTORIALEVENTSLISTENER_H__