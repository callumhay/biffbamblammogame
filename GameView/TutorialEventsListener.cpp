/**
 * TutorialEventsListener.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TutorialEventsListener.h"
#include "GameDisplay.h"

#include "../GameModel/GameItem.h"
#include "../GameModel/BallBoostModel.h"

TutorialEventsListener::TutorialEventsListener(GameDisplay* display) : display(display),
numBlocksDestroyed(0), movePaddleHint(NULL), movePaddleHintUnshown(false), fireWeaponAlreadyShown(false),
shootBallHint(NULL), fireWeaponHint(NULL), startBoostHint(NULL), doBoostHint(NULL), holdBoostHint(NULL) {
    assert(display != NULL);
}

TutorialEventsListener::~TutorialEventsListener() {
}


void TutorialEventsListener::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    switch (pressedButton) {

        case GameControl::LeftButtonAction:
        case GameControl::RightButtonAction:
            if (!this->movePaddleHintUnshown) {
                this->movePaddleHint->Unshow(0.0, 0.5);
                this->movePaddleHintUnshown = true;
                this->shootBallHint->Show(0.5, 0.5);
            }
            break;

        default:
            break;
    }
}

void TutorialEventsListener::MousePressed(const GameControl::MouseButton& pressedButton) {
    UNUSED_PARAMETER(pressedButton);
}

void TutorialEventsListener::ItemActivatedEvent(const GameItem& item) {
    switch (item.GetItemType()) {
        case GameItem::LaserBulletPaddleItem:
        case GameItem::LaserBeamPaddleItem:
        case GameItem::RocketPaddleItem:
            if (!this->fireWeaponAlreadyShown) {
                this->fireWeaponHint->Show(0.0, 1.0);
                this->fireWeaponHint->Unshow(6.0, 1.0);
                this->fireWeaponAlreadyShown = true;
            }
            break;
        default:
            break;
    
    }
}

void TutorialEventsListener::BallBoostGainedEvent() {
    const BallBoostModel* boostModel = this->display->GetModel()->GetBallBoostModel();
    if (boostModel == NULL) { return; }
    
    if (boostModel->GetNumAvailableBoosts() == 1) {
        this->startBoostHint->Show(0.0, 0.5);   
    }
}

void TutorialEventsListener::BulletTimeStateChangedEvent(const BallBoostModel& boostModel) {
    switch (boostModel.GetBulletTimeState()) {
        case BallBoostModel::NotInBulletTime:
            //if (boostModel.GetNumAvailableBoosts() == 1) {
            //}
            break;

        case BallBoostModel::BulletTimeFadeIn:
            // Show the hint for telling the user how to boost the ball while in bullet time
            this->startBoostHint->Unshow(0.0, 0.5);
            this->doBoostHint->Show(0.0, 0.5);
            this->holdBoostHint->Show(0.0, 0.5);
        case BallBoostModel::BulletTime:

            break;

        case BallBoostModel::BulletTimeFadeOut:
            this->doBoostHint->Unshow(0.0, 0.5);
            this->holdBoostHint->Unshow(0.0, 0.5);
            break;
        default:
            assert(false);
            break;
    }
}

void TutorialEventsListener::LivesChangedEvent(int livesLeftBefore, int livesLeftAfter) {
    if (livesLeftBefore != livesLeftAfter) {

        // We don't let the player lose or gain any lives - i.e., they sustain the starting
        // number of lives throughout the tutorial
        GameModel* gameModel = this->display->GetModel();

        // Since the call to "SetLivesLeft" causes this event to be called again we need
        // to tell the method not to signal any further events
	    gameModel->SetLivesLeft(livesLeftBefore, false);
    }
}