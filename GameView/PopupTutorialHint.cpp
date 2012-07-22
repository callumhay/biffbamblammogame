/**
 * PopupTutorialHint.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PopupTutorialHint.h"
#include "TutorialHintListeners.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../GameModel/GameModel.h"

const double PopupTutorialHint::UNSHOW_TIME = 0.33;

PopupTutorialHint::PopupTutorialHint(GameModel* gameModel, size_t width) : 
TutorialHint(), gameModel(gameModel), pane(NULL), timeToUnshow(0.0), paneHandler(NULL) {

    assert(gameModel != NULL);
    this->paneHandler = new PaneHandler(this);
    this->pane = new DecoratorOverlayPane(this->paneHandler, width, 
        GameViewConstants::GetInstance()->TUTORIAL_PANE_COLOUR);
    
}

PopupTutorialHint::~PopupTutorialHint() {
    delete this->pane;
    this->pane = NULL;
    delete this->paneHandler;
    this->paneHandler = NULL;
}

void PopupTutorialHint::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    this->pane->ButtonPressed(pressedButton);
}

void PopupTutorialHint::Show(double delayInSeconds, double fadeInTimeInSeconds) {
    UNUSED_PARAMETER(delayInSeconds);
    if (this->isShown) { return; }
    
    this->pane->Show(fadeInTimeInSeconds);
    if (this->listener != NULL) {
        this->listener->OnTutorialHintShown();
    }

    // Pause the game
    this->gameModel->SetPause(GameModel::PauseGame);

    this->isShown = true;
}

void PopupTutorialHint::Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow) {
    UNUSED_PARAMETER(delayInSeconds);
    if (!this->isShown && !overridePrevUnshow) { return; }
    
    if (!this->pane->IsFinished()) {
        this->pane->Hide(fadeOutTimeInSeconds);
    }
    if (this->listener != NULL) {
        this->listener->OnTutorialHintUnshown();
    }

    // Unpause the game
    this->gameModel->UnsetPause(GameModel::PauseGame);

    this->isShown = false;
}

void PopupTutorialHint::Draw(double dT, const Camera& camera) {
    this->pane->Draw(dT, camera.GetWindowWidth(), camera.GetWindowHeight());

    if (this->pane->IsFinished()) {
        if (timeToUnshow >= UNSHOW_TIME) {
            this->Unshow(0.0, 0.0);
        }
        timeToUnshow += dT;
    }
}

// Event handler for when an option is selected in the pop-up hint's pane
void PopupTutorialHint::PaneHandler::OptionSelected(const std::string& optionText) {
    UNUSED_PARAMETER(optionText);
    this->popupHint->GetPane()->Hide(1.0);
}