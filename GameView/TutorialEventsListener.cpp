/**
 * TutorialEventsListener.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "TutorialEventsListener.h"
#include "GameDisplay.h"

#include "../GameModel/GameItem.h"
#include "../GameModel/BallBoostModel.h"

TutorialEventsListener::TutorialEventsListener(GameDisplay* display) : display(display),
numBlocksDestroyed(0), movePaddleHint(NULL), movePaddleHintUnshown(false), fireWeaponAlreadyShown(false),
finishedPointsHint(false), keepShowingBoostHint(true), shootBallHint(NULL), fireWeaponHint(NULL), startBoostHint(NULL), 
doBoostPressToReleaseHint(NULL), doBoostSlingshotHint(NULL), holdBoostHint(NULL), hasShownBoostHint(false),
boostAvailableHint(NULL), multiplierHints(NULL), multiplierLostHint(NULL) {
    assert(display != NULL);
}

TutorialEventsListener::~TutorialEventsListener() {
}

void TutorialEventsListener::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    
    switch (pressedButton) {

        case GameControl::LeftButtonAction:
        case GameControl::RightButtonAction:
            
            if (!this->movePaddleHintUnshown) {
                this->movePaddleHint->Unshow(0.0, 0.25);
                this->movePaddleHintUnshown = true;
                this->shootBallHint->Show(0.25, 0.25);
            }
            break;

        default:
            break;
    }
}

void TutorialEventsListener::MousePressed(const GameControl::MouseButton& pressedButton) {
    UNUSED_PARAMETER(pressedButton);
}

void TutorialEventsListener::BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle, bool hitPaddleUnderside) {
    UNUSED_PARAMETER(ball);
    UNUSED_PARAMETER(paddle);
    UNUSED_PARAMETER(hitPaddleUnderside);

    if (this->multiplierHints->IsDoneShowingAllHints() && 
        !this->multiplierLostHint->IsDoneShowingAllHints()) {

        if (this->multiplierLostHint->GetIsVisible()) {
            this->multiplierLostHint->Resume(1.0);
        }
        else {
            this->multiplierLostHint->Show(0.0, 1.0);
        }
    }
}

void TutorialEventsListener::BlockDestroyedEvent(const LevelPiece& block, const LevelPiece::DestructionMethod& method) {
    UNUSED_PARAMETER(block);
    UNUSED_PARAMETER(method);

    this->numBlocksDestroyed++;

    const GameModel* gameModel = this->display->GetModel();
    const GameLevel* level = gameModel->GetCurrentLevel();

    // Check to see whether the points tutorial hint has already been faded...
    if (!this->finishedPointsHint) {
        // Figure out when to fade the points hint...
        
        // The following block indices must be empty to make the points hint disappear:
        // row idx 14 - 16
        // col idx 1 - 8
        
        const std::vector<std::vector<LevelPiece*> >& levelPieces = level->GetCurrentLevelLayout();
        bool blockIndicesAllEmpty = true;
        for (int row = 14; row <= 16 && blockIndicesAllEmpty; row++) {
            for (int col = 1; col <= 8 && blockIndicesAllEmpty; col++) {
                if (levelPieces[row][col]->GetType() != LevelPiece::Empty) {
                    blockIndicesAllEmpty = false;
                }
            }
        }

        if (blockIndicesAllEmpty) {
            //this->pointsTutorialHintEmitter->SetParticleLife(ESPInterval(3.0f), true);
            //this->pointsTutorialHintEmitter->ClearEffectors();
            //this->pointsTutorialHintEmitter->AddEffector(&this->fadeEffector);
            this->finishedPointsHint = true;
        }
    }
    
    // Determine the state of the multiplier hint and display it appropriately
    if (this->multiplierHints->GetIsVisible() && this->hasShownBoostHint &&
        gameModel->GetBallBoostModel() != NULL && gameModel->GetBallBoostModel()->GetBulletTimeState() == BallBoostModel::NotInBulletTime) {
        this->multiplierHints->Resume(1.0);
    }
    else {
        GameBall* ball = this->display->GetModel()->GetGameBalls().front();
        assert(ball != NULL);

        if (!this->multiplierHints->IsDoneShowingAllHints() &&
            !level->IsLevelAlmostComplete() && this->hasShownBoostHint && 
            ball->GetCenterPosition2D()[1] >= 18 * LevelPiece::PIECE_HEIGHT) {

            // Start showing the multiplier hints...
            if (!this->multiplierHints->GetIsVisible()) {
                this->multiplierHints->Show(0.0, 1.0);
            }
        }
    }
}

void TutorialEventsListener::ItemActivatedEvent(const GameItem& item) {
    switch (item.GetItemType()) {
        case GameItem::LaserBulletPaddleItem:
        case GameItem::LaserBeamPaddleItem:
        case GameItem::RocketPaddleItem:
        case GameItem::MineLauncherPaddleItem:
        case GameItem::RemoteCtrlRocketItem:
            if (!this->fireWeaponAlreadyShown && !this->fireWeaponHint->GetIsVisible() && 
                this->hasShownBoostHint && !this->startBoostHint->GetIsVisible()) {
                this->fireWeaponHint->Show(0.0, 1.0); 
            }
            break;
        default:
            break;
    }
}

void TutorialEventsListener::PaddleWeaponFiredEvent() {
    if (!this->fireWeaponAlreadyShown) {
        this->fireWeaponHint->Unshow(0.0, 0.5);
        this->fireWeaponAlreadyShown = true;
    }
}

void TutorialEventsListener::BallBoostGainedEvent() {
    const BallBoostModel* boostModel = this->display->GetModel()->GetBallBoostModel();
    if (boostModel == NULL) { return; }
    
    if (boostModel->GetNumAvailableBoosts() == 1 && this->keepShowingBoostHint) {
        this->startBoostHint->Show(0.0, 0.5);
        this->boostAvailableHint->Show(0.0, 0.5);
        this->hasShownBoostHint = true;
    }
}

void TutorialEventsListener::BallBoostLostEvent(bool allBoostsLost) {
    
    if (allBoostsLost) {
        this->startBoostHint->Unshow(0.0, 0.5);
        this->boostAvailableHint->Unshow(0.0, 0.5);
    }
}

void TutorialEventsListener::BallBoostUsedEvent() {
    if (this->hasShownBoostHint) {
        this->keepShowingBoostHint = false;
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
            if (this->keepShowingBoostHint) {
                if (this->display->GetModel()->GetBallBoostMode() == BallBoostModel::Slingshot) {
                    this->doBoostSlingshotHint->Show(0.0, 0.5);
                }
                else {
                    this->doBoostPressToReleaseHint->Show(0.0, 0.5);
                }
                this->holdBoostHint->Show(0.0, 0.5);
            }
            this->boostAvailableHint->Unshow(0.0, 0.5);

            if (this->multiplierHints->GetIsVisible()) {
                this->multiplierHints->Pause(0.5);
            }
            if (this->multiplierLostHint->GetIsVisible()) {
                this->multiplierLostHint->Pause(0.5);
            }
            this->fireWeaponHint->Unshow(0.0, 0.5);

            break;

        case BallBoostModel::BulletTime:

            break;

        case BallBoostModel::BulletTimeFadeOut:
            this->doBoostSlingshotHint->Unshow(0.0, 0.5);
            this->doBoostPressToReleaseHint->Unshow(0.0, 0.5);
            this->holdBoostHint->Unshow(0.0, 0.5);

            if (boostModel.GetNumAvailableBoosts() > 0 && this->keepShowingBoostHint) {
                this->boostAvailableHint->Show(0.0, 0.5);
            }

            if (this->display->GetModel()->GetPlayerPaddle()->HasPaddleType(
                PlayerPaddle::LaserBeamPaddle | PlayerPaddle::LaserBulletPaddle | PlayerPaddle::RocketPaddle) &&
                !this->fireWeaponAlreadyShown) {

                this->fireWeaponHint->Show(0.0, 1.0);
            }

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

void TutorialEventsListener::AllBallsDeadEvent(int livesLeft) {
    UNUSED_PARAMETER(livesLeft);

    // If all balls died then we should unshow all the tutorial hints for ball boosting
    this->startBoostHint->Unshow(0.0, 0.5);
    this->doBoostSlingshotHint->Unshow(0.0, 0.5);
    this->doBoostPressToReleaseHint->Unshow(0.0, 0.5);
    this->holdBoostHint->Unshow(0.0, 0.5);
    this->fireWeaponHint->Unshow(0.0, 0.5);
    this->multiplierHints->Pause(0.5);
    this->multiplierLostHint->Pause(0.5);
}

void TutorialEventsListener::LastBallAboutToDieEvent(const GameBall& lastBallToDie) {
    UNUSED_PARAMETER(lastBallToDie);

    // If all balls died (or are about to) then we should unshow all the tutorial hints for ball boosting
    this->startBoostHint->Unshow(0.0, 0.5);
    this->doBoostSlingshotHint->Unshow(0.0, 0.5);
    this->doBoostPressToReleaseHint->Unshow(0.0, 0.5);
    this->holdBoostHint->Unshow(0.0, 0.5);
    this->fireWeaponHint->Unshow(0.0, 0.5);
    this->multiplierHints->Pause(0.5);
    this->multiplierLostHint->Pause(0.5);
}

void TutorialEventsListener::NumStarsChangedEvent(const PointAward* pointAward, int oldNumStars, int newNumStars) {
    UNUSED_PARAMETER(pointAward);
    UNUSED_PARAMETER(oldNumStars);
    UNUSED_PARAMETER(newNumStars);
}