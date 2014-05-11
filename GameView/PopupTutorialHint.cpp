/**
 * PopupTutorialHint.cpp
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

#include "PopupTutorialHint.h"
#include "TutorialHintListeners.h"
#include "GameViewConstants.h"


#include "../GameModel/GameModel.h"

const double PopupTutorialHint::UNSHOW_TIME = 0.33;

PopupTutorialHint::PopupTutorialHint(size_t width) : 
TutorialHint(), pane(NULL), timeToUnshow(0.0), paneHandler(NULL) {

    this->paneHandler = new PaneHandler(this);
    this->pane = new DecoratorOverlayPane(this->paneHandler, width, 
        GameViewConstants::GetInstance()->TUTORIAL_PANE_COLOUR);
    
}

PopupTutorialHint::PopupTutorialHint(size_t width, PaneHandler* handler) :
TutorialHint(), pane(NULL), timeToUnshow(0.0), paneHandler(NULL) {
    handler->SetPopupTutorialHint(this);
    this->paneHandler = handler;
    
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
    
    this->pane->Show(delayInSeconds, fadeInTimeInSeconds);
    if (this->listener != NULL) {
        this->listener->OnTutorialHintShown();
    }

    this->isShown = true;
}

void PopupTutorialHint::Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow) {
    if (!this->isShown && !overridePrevUnshow) { return; }
    
    if (!this->pane->IsFinished()) {
        this->pane->Hide(delayInSeconds, fadeOutTimeInSeconds);
    }
    if (this->listener != NULL) {
        this->listener->OnTutorialHintUnshown();
    }

    this->isShown = false;
}

void PopupTutorialHint::Tick(double dT) {
    this->pane->Tick(dT);

    if (this->pane->IsFinished()) {
        if (this->timeToUnshow >= UNSHOW_TIME) {
            this->Unshow(0.0, 0.0);
        }
        timeToUnshow += dT;
    }
}

// Event handler for when an option is selected in the pop-up hint's pane
void PaneHandler::OptionSelected(const std::string& optionText) {
    UNUSED_PARAMETER(optionText);
    this->popupHint->GetPane()->Hide(0.0, 1.0);
}