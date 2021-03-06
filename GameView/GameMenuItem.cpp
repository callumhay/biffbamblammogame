/**
 * GameMenuItem.cpp
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

#include "GameMenuItem.h"
#include "GameMenu.h"
#include "GameViewConstants.h"
#include "GameDisplay.h"

#include "../BlammoEngine/Camera.h"

const float GameMenuItem::MENU_ITEM_WOBBLE_AMT_LARGE = 8.0f;
const float GameMenuItem::MENU_ITEM_WOBBLE_AMT_SMALL = 4.0f;
const float GameMenuItem::MENU_ITEM_WOBBLE_FREQ      = 0.2f;
const float GameMenuItem::SUB_MENU_PADDING           = 15.0f;

// GameMenuItem Functions *******************************************

GameMenuItem::GameMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameSubMenu* subMenu) : 
smTextLabel(smLabel), lgTextLabel(lgLabel), subMenu(subMenu), eventHandler(NULL), isActivated(false) {
	
	assert(smLabel.GetText() == lgLabel.GetText());

	this->currLabel = &this->smTextLabel;
	this->wiggleAnimation.SetInterpolantValue(0.0f);
}

GameMenuItem::~GameMenuItem() {
}

// Return the height of this menu item
unsigned int GameMenuItem::GetHeight() const {
	return this->currLabel->GetHeight();
}

float GameMenuItem::GetWidth(bool useMax) const {
    UNUSED_PARAMETER(useMax);
	return this->currLabel->GetLastRasterWidth();
}

/**
 * Draw this menu item on the screen with the given coordinate
 * point specifying the top-left corner location where it will
 * be drawn, in window coordinates.
 */
void GameMenuItem::Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight) {
	UNUSED_PARAMETER(windowWidth);
	UNUSED_PARAMETER(windowHeight);

	float wiggleAmount = this->wiggleAnimation.GetInterpolantValue();

	this->currLabel->SetTopLeftCorner(topLeftCorner + Vector2D(wiggleAmount, 0.0f));
	this->currLabel->Draw(true);

	// Animate the wiggle and/or pulse (if there are any animations loaded)
	this->wiggleAnimation.Tick(dT);

	if (subMenu != NULL) {
		const float PAD_AMT = GameMenuItem::MENU_ITEM_WOBBLE_AMT_LARGE + GameMenuItem::SUB_MENU_PADDING + GameMenu::BACKGROUND_PADDING + 2*GameSubMenu::HALF_ARROW_WIDTH;
		subMenu->SetTopLeftCorner(topLeftCorner + Vector2D(this->lgTextLabel.GetLastRasterWidth() + PAD_AMT, 0.0f));
	}
}

/** 
 * Toggles the wiggle animation for this menu item on with the
 * given amplitude and frequency of wiggle given.
 */
void GameMenuItem::ToggleWiggleAnimationOn(float amplitude, float frequency) {

	std::vector<float> wiggleValues;
	wiggleValues.reserve(5);
	wiggleValues.push_back(0.0f);
	wiggleValues.push_back(amplitude);
	wiggleValues.push_back(0.0f);
	wiggleValues.push_back(-amplitude);
	wiggleValues.push_back(0.0f);

	std::vector<double> timeValues;
	timeValues.reserve(5);
	timeValues.push_back(0.0);
	timeValues.push_back(frequency);
	timeValues.push_back(2*frequency);
	timeValues.push_back(3*frequency);
	timeValues.push_back(4*frequency);

	this->wiggleAnimation.SetLerp(timeValues, wiggleValues);
	this->wiggleAnimation.SetRepeat(true);
}

void GameMenuItem::ToggleWiggleAnimationOff() {
	this->wiggleAnimation.ClearLerp();
	this->wiggleAnimation.SetRepeat(false);
	this->wiggleAnimation.SetInterpolantValue(0.0f);
}

// GameMenuItemWithFlashLabel Functions ********************************************************************

GameMenuItemWithFlashLabel::GameMenuItemWithFlashLabel(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, 
                                                       const TextLabel2D& flashLabel, GameSubMenu* subMenu) :
GameMenuItem(smLabel, lgLabel, subMenu), flashLbl(flashLabel) {

    this->flashAnimation = GameViewConstants::GetInstance()->BuildFlashingColourAnimation();
}

GameMenuItemWithFlashLabel::~GameMenuItemWithFlashLabel() {
}

void GameMenuItemWithFlashLabel::Draw(double dT, const Point2D& topLeftCorner,
                                      int windowWidth, int windowHeight) {
    GameMenuItem::Draw(dT, topLeftCorner, windowWidth, windowHeight);

    // Draw flashy label...
    this->flashAnimation.Tick(dT);
    this->flashLbl.SetColour(this->flashAnimation.GetInterpolantValue());
    this->flashLbl.SetTopLeftCorner(this->currLabel->GetTopLeftCorner() + Vector2D(this->currLabel->GetLastRasterWidth() + 10, 0));
    this->flashLbl.Draw();
}

// SelectionListMenuItem Functions ***************************************************************************

const float SelectionListMenuItem::INTERIOR_PADDING				= 10.0f;  // The padding between the text and arrows in the item
const float SelectionListMenuItem::SELECTION_ARROW_WIDTH	= 15.0f;	// The width of the arrows for indicating movement through the list

SelectionListMenuItem::SelectionListMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, const std::vector<std::string>& items) :
GameMenuItem(smLabel, lgLabel, NULL), selectionList(items), selectedIndex(SelectionListMenuItem::NO_SELECTION),
previouslySelectedIndex(SelectionListMenuItem::NO_SELECTION), baseLabelStr(lgLabel.GetText()), maxWidth(0.0f) {

	this->SetSelectionList(items);
}

SelectionListMenuItem::~SelectionListMenuItem() {
}

float SelectionListMenuItem::GetWidth(bool useMax) const {
    if (useMax) {
        return this->maxWidth;
    }
    else {
        float width = 0.0f;
        this->currLabel->SetText(this->baseLabelStr);
        width += this->currLabel->GetLastRasterWidth();
	    width += 2 * SelectionListMenuItem::INTERIOR_PADDING + 
		    2 * SelectionListMenuItem::SELECTION_ARROW_WIDTH;
        this->currLabel->SetText(this->selectionList[this->selectedIndex]);
        width += this->currLabel->GetLastRasterWidth();
        return width;
    }
}

/**
 * Sets the list of selectable items in this menu item.
 */
void SelectionListMenuItem::SetSelectionList(const std::vector<std::string>& items) {
	if (items.size() > 0 && this->selectedIndex == SelectionListMenuItem::NO_SELECTION) {
		this->selectedIndex = 0;
	}
	this->selectionList = items;

	// Try to figure out what the maximum width of this item is
	this->lgTextLabel.SetText(this->baseLabelStr);
	const float BASE_LABEL_WIDTH = this->lgTextLabel.GetLastRasterWidth();
	
	TextLabel2D tempLabel = this->lgTextLabel;
	for (size_t i = 0; i < items.size(); i++) {
		
		tempLabel.SetText(this->selectionList[i]);
		const float CURR_ITEM_WIDTH = BASE_LABEL_WIDTH + 2 * SelectionListMenuItem::INTERIOR_PADDING + 
		    2 * SelectionListMenuItem::SELECTION_ARROW_WIDTH + tempLabel.GetLastRasterWidth();
		this->maxWidth = std::max<float>(this->maxWidth, CURR_ITEM_WIDTH);
	}
}

void SelectionListMenuItem::Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight) {
	UNUSED_PARAMETER(windowWidth);
	UNUSED_PARAMETER(windowHeight);

	// Obtain the latest interpolated wiggle value
	float wiggleAmount = this->wiggleAnimation.GetInterpolantValue();
	
	// Animate the wiggle and/or pulse (if there are any animations loaded)
	this->wiggleAnimation.Tick(dT);

	// Draw the base label portion of the item
	Point2D wiggleTopLeftCorner = topLeftCorner + Vector2D(wiggleAmount, 0.0f);
	this->currLabel->SetText(this->baseLabelStr);
	this->currLabel->SetTopLeftCorner(wiggleTopLeftCorner);
	this->currLabel->Draw(true);

	// Exit after the base label if there's no item selected
	if (this->selectedIndex == NO_SELECTION || this->selectionList.size() == 0) {
		return;
	}

	// Draw the currently selected item with arrows on either side of it
	const float BASE_LABEL_WIDTH = this->currLabel->GetLastRasterWidth();
	wiggleTopLeftCorner = wiggleTopLeftCorner + Vector2D(BASE_LABEL_WIDTH + SelectionListMenuItem::INTERIOR_PADDING, 0.0f);

	// Draw the left pointing arrow
	this->DrawSelectionArrow(wiggleTopLeftCorner, this->currLabel->GetHeight(), true);
	wiggleTopLeftCorner = wiggleTopLeftCorner + Vector2D(SelectionListMenuItem::SELECTION_ARROW_WIDTH + SelectionListMenuItem::INTERIOR_PADDING, 0.0f);

	// Draw the text for the currently selected item
	const std::string& CURR_SEL_ITEM_STR = this->selectionList[this->selectedIndex];
	this->currLabel->SetText(CURR_SEL_ITEM_STR);
	this->currLabel->SetTopLeftCorner(wiggleTopLeftCorner);
	this->currLabel->Draw(true);

	const float SEL_ITEM_LABEL_WIDTH = this->currLabel->GetLastRasterWidth();
	wiggleTopLeftCorner = wiggleTopLeftCorner + Vector2D(SEL_ITEM_LABEL_WIDTH + SelectionListMenuItem::INTERIOR_PADDING, 0.0f);

	// Draw the right pointing arrow
	this->DrawSelectionArrow(wiggleTopLeftCorner, this->currLabel->GetHeight(), false);
}

/**
 * Private helper function for drawing the selection arrows for the item.
 */
void SelectionListMenuItem::DrawSelectionArrow(const Point2D& topLeftCorner, float arrowHeight, bool isLeftPointing) {
	const float HALF_ARROW_HEIGHT = arrowHeight / 2.0f;
	const float HALF_ARROW_WIDTH	= SelectionListMenuItem::SELECTION_ARROW_WIDTH / 2.0f;
	
	// Arrow vertices, centered on the origin
	const Point2D APEX_POINT	= (isLeftPointing ? Point2D(-HALF_ARROW_WIDTH, 0.0f) : Point2D(HALF_ARROW_WIDTH, 0.0f));
	const Point2D TOP_POINT		= (isLeftPointing ? Point2D(HALF_ARROW_WIDTH, HALF_ARROW_HEIGHT) : Point2D(-HALF_ARROW_WIDTH, HALF_ARROW_HEIGHT));
	const Point2D BOTTOM_POINT	= (isLeftPointing ? Point2D(HALF_ARROW_WIDTH, -HALF_ARROW_HEIGHT) : Point2D(-HALF_ARROW_WIDTH, -HALF_ARROW_HEIGHT));

	// We set the alpha of the arrows based on the text
	const ColourRGBA& currTextColour = this->currLabel->GetColour();

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_POLYGON_BIT);
	Camera::PushWindowCoords();
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
    glLoadIdentity();
	glTranslatef(topLeftCorner[0] + HALF_ARROW_WIDTH, topLeftCorner[1] - HALF_ARROW_HEIGHT, 0.0f);

	// Fill in the arrows
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(1.0f, 0.6f, 0.0f, currTextColour.A());
	
	if (isLeftPointing) {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
		glEnd();
	}
	else {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
		glEnd();
	}
	
	// Draw the outlines of the arrows
	glPolygonMode(GL_FRONT, GL_LINE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(2.0f);
	glColor4f(0.0f, 0.0f, 0.0f, currTextColour.A());

	if (isLeftPointing) {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
		glEnd();
	}
	else {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
		glEnd();
	}

	glPopMatrix();
	Camera::PopWindowCoords();
	glPopAttrib();
}

/**
 * Called from the parent menu of this item when a key is pressed and this item
 * is the one currently selected and relevant.
 */
void SelectionListMenuItem::ButtonPressed(const GameControl::ActionButton& pressedButton, 
                                          const GameControl::ActionMagnitude& magnitude) {
    UNUSED_PARAMETER(magnitude);
	assert(this->parent != NULL);

	// Key pressing does nothing if there's nothing to select from
	if (this->selectionList.size() == 0) {
		return;
	}

	switch (pressedButton) {
		case GameControl::LeftButtonAction:
			// Move the selection down one item
			this->selectedIndex--;
			if (this->selectedIndex < 0) {
				this->selectedIndex = this->selectionList.size() - 1;
			}

			if (this->eventHandler != NULL) {
				this->eventHandler->MenuItemScrolled();
			}
			break;

		case GameControl::RightButtonAction:
			// Move the selection up one item
			this->selectedIndex = (this->selectedIndex + 1) % this->selectionList.size();
			
			if (this->eventHandler != NULL) {
				this->eventHandler->MenuItemScrolled();
			}
			break;

		case GameControl::EnterButtonAction:
            this->EscapeFromMenuItem(true);
			break;

        case GameControl::UpButtonAction:
            //this->EscapeFromMenuItem(true);
            //if (this->parent != NULL) {
            //    this->parent->ButtonPressed(GameControl::UpButtonAction);
            //}
            break;

        case GameControl::DownButtonAction:
            //this->EscapeFromMenuItem(true);
            //if (this->parent != NULL) {
            //    this->parent->ButtonPressed(GameControl::DownButtonAction);
            //}
            break;

		case GameControl::EscapeButtonAction:
			this->EscapeFromMenuItem(false);
			break;

		default:
			break;
	}

	assert(this->selectedIndex >= 0 && this->selectedIndex < static_cast<int>(this->selectionList.size()));
}

void SelectionListMenuItem::EscapeFromMenuItem(bool saveChange) {
	
    if (saveChange) {
		// When return is hit, the value is locked and results in a change in the menu item
		// tell the parent about this in order to send out a change event
		if (this->parent != NULL) {
			if (this->selectedIndex != this->previouslySelectedIndex) {
				this->parent->ActivatedMenuItemChanged();
			}
			this->parent->DeactivateSelectedMenuItem();
		}
		if (this->eventHandler != NULL) {
			this->eventHandler->MenuItemConfirmed();
		}
    }
    else {
        // If the user hits ESC then we sure the selection inside the item doesn't change
	    this->selectedIndex = this->previouslySelectedIndex;
	    if (this->parent != NULL) {
		    this->parent->DeactivateSelectedMenuItem();
	    }
	    if (this->eventHandler != NULL) {
		    this->eventHandler->MenuItemCancelled();
	    }
    }
}

/**
 * Tell this menu item to become selected. This function must be called
 * in order to ensure this item is properly activated.
 */
void SelectionListMenuItem::Activate() {
    GameMenuItem::Activate();
	// Set the previous index to the current one (so we keep track of it)
	this->previouslySelectedIndex = this->selectedIndex;
}

// SelectionListMenuItemWithVerify Functions **********************************************************

SelectionListMenuItemWithVerify::SelectionListMenuItemWithVerify(const TextLabel2D& smLabel, const TextLabel2D& lgLabel,
                                                                 const std::vector<std::string>& items, VerifyMenuItem* verifyMenu) :
SelectionListMenuItem(smLabel, lgLabel, items), verifyMenu(verifyMenu), verifyMenuActive(false) {
    assert(verifyMenu != NULL);
    this->verifyMenu->SetParent(this);
}

SelectionListMenuItemWithVerify::~SelectionListMenuItemWithVerify() {
    delete this->verifyMenu;
    this->verifyMenu = NULL;
}

void SelectionListMenuItemWithVerify::Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight) {
    SelectionListMenuItem::Draw(dT, topLeftCorner, windowWidth, windowHeight);
    if (this->verifyMenuActive) {
        this->verifyMenu->Draw(dT, topLeftCorner, windowWidth, windowHeight);
    }
}

void SelectionListMenuItemWithVerify::ButtonPressed(const GameControl::ActionButton& pressedButton, 
                                                    const GameControl::ActionMagnitude& magnitude) {

    if (this->verifyMenuActive) {
        this->verifyMenu->ButtonPressed(pressedButton, magnitude);
    }
    else {
        SelectionListMenuItem::ButtonPressed(pressedButton, magnitude);
    }
}

void SelectionListMenuItemWithVerify::ButtonReleased(const GameControl::ActionButton& releasedButton) {
    if (this->verifyMenuActive) {
        this->verifyMenu->ButtonReleased(releasedButton);
    }
    else {
        SelectionListMenuItem::ButtonReleased(releasedButton);
    }
}

void SelectionListMenuItemWithVerify::EscapeFromMenuItem(bool saveChange) {
    if (saveChange) {
        if (this->verifyMenuActive) {
            assert(this->verifyMenu->GetSelectedVerifyMenuOption() == VerifyMenuItem::Confirm);
            SelectionListMenuItem::EscapeFromMenuItem(true);
        }
        else {
            // Check to see if the selected option is the same as what it was before the menu was opened, if
            // so then we just escape from the menu without saving changes
            if (this->previouslySelectedIndex == this->selectedIndex) {
                SelectionListMenuItem::EscapeFromMenuItem(true);
            }
            else {
                // Open up the verify menu...
                this->verifyMenuActive = true;
                this->verifyMenu->SetSelectedVerifyMenuOption(VerifyMenuItem::Cancel);
                this->verifyMenu->Activate();
            }
        }
    }
    else {
        SelectionListMenuItem::EscapeFromMenuItem(false);
    }
}

void SelectionListMenuItemWithVerify::DeactivateSelectedMenuItem() {
    assert(this->verifyMenuActive);

    if (this->verifyMenu->GetSelectedVerifyMenuOption() == VerifyMenuItem::Confirm) {
        this->EscapeFromMenuItem(true);
    }
    this->verifyMenuActive = false;
    this->verifyMenu->Deactivate();
}

// AmountScrollerMenuItem Functions *******************************************************************

// The distance between the text and arrows 
const float AmountScrollerMenuItem::INTERIOR_PADDING_TEXT_ARROWS = 10.0f;
// The distance between the arrows and scroller
const float AmountScrollerMenuItem::INTERIOR_PADDING_ARROWS_SCROLLER = 5.0f;
// Width of the arrows on either side of the scroller widget
const float AmountScrollerMenuItem::SCROLLER_ARROW_WIDTH = 10.0f;
// Width of the value scroller widget in pixels
const float AmountScrollerMenuItem::SCROLLER_WIDTH = 180.0f;

// Time between pulses of incrementing when the increment button is being held
// down by the user
const double AmountScrollerMenuItem::INCREMENT_PULSE_TIME = 0.05;

AmountScrollerMenuItem::AmountScrollerMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, 
                                               float minValue, float maxValue, float currentValue, float incrementAmt) :
GameMenuItem(smLabel, lgLabel, NULL),
isActive(false),
minValue(minValue),
maxValue(maxValue),
currentValue(currentValue),
previouslySelectedValue(currentValue),
incrementAmt(incrementAmt),
baseIncrementAmt(incrementAmt),
baseLabelStr(lgLabel.GetText()),
alwaysSendChangeUpdates(false),
increaseValueButtonPressed(false),
decreaseValueButtonPressed(false)
{
	this->lgTextLabel.SetText(this->baseLabelStr);
	this->maxWidth = this->lgTextLabel.GetLastRasterWidth() + AmountScrollerMenuItem::INTERIOR_PADDING_TEXT_ARROWS + 
		2 * AmountScrollerMenuItem::SCROLLER_ARROW_WIDTH + 2 * AmountScrollerMenuItem::INTERIOR_PADDING_ARROWS_SCROLLER +
		AmountScrollerMenuItem::SCROLLER_WIDTH;
}

AmountScrollerMenuItem::~AmountScrollerMenuItem() {
}

void AmountScrollerMenuItem::Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight) {
	UNUSED_PARAMETER(windowWidth);
	UNUSED_PARAMETER(windowHeight);

	// Obtain the latest interpolated wiggle value
	float wiggleAmount = this->wiggleAnimation.GetInterpolantValue();
	
	// Animate the wiggle and/or pulse (if there are any animations loaded)
	this->wiggleAnimation.Tick(dT);

	// Draw the base label portion of the item
	Point2D wiggleTopLeftCorner = topLeftCorner + Vector2D(wiggleAmount, 0.0f);
	this->currLabel->SetText(this->baseLabelStr);
	this->currLabel->SetTopLeftCorner(wiggleTopLeftCorner);
	this->currLabel->Draw();

	// If the item is not active then we don't draw any more of it
	//if (!this->isActive) {
	//	return;
	//}

	// Update the amount to increment by...
	static double timeCounter = 0.0;
	timeCounter += dT;
	if (timeCounter >= AmountScrollerMenuItem::INCREMENT_PULSE_TIME) {
		timeCounter = 0.0;
		
		if (this->increaseValueButtonPressed) {
			this->ChangeScrollerValue(this->incrementAmt, true);
		}
		else if (this->decreaseValueButtonPressed) {
			this->ChangeScrollerValue(-this->incrementAmt, true);
		}
	}

	Camera::PushWindowCoords();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_POLYGON_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

	// Draw the left pointing scroller arrow
	const float BASE_LABEL_WIDTH  = this->currLabel->GetLastRasterWidth();
	const float SCROLLER_HEIGHT = this->currLabel->GetHeight();

	wiggleTopLeftCorner = wiggleTopLeftCorner + Vector2D(BASE_LABEL_WIDTH + AmountScrollerMenuItem::INTERIOR_PADDING_TEXT_ARROWS, 0);
	this->DrawScrollerArrow(wiggleTopLeftCorner, SCROLLER_HEIGHT, true);

	// Draw the scroller widget
	wiggleTopLeftCorner = wiggleTopLeftCorner + Vector2D(AmountScrollerMenuItem::SCROLLER_ARROW_WIDTH + AmountScrollerMenuItem::INTERIOR_PADDING_ARROWS_SCROLLER, 0);
	
	const float NEW_Y_POS = wiggleTopLeftCorner[1];

	const float SCROLLER_X_WIDTH		  = wiggleTopLeftCorner[0] + SCROLLER_WIDTH;
	const float SCROLLER_Y_HEIGHT		  = NEW_Y_POS - SCROLLER_HEIGHT;
	const float SCROLLER_FILL_X_WIDTH = wiggleTopLeftCorner[0] + SCROLLER_WIDTH * ((this->currentValue - this->minValue) / (this->maxValue - this->minValue));
	
    glPolygonMode(GL_FRONT, GL_FILL);
	glBegin(GL_QUADS);
	
	// Shadowy background
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	glVertex2f(wiggleTopLeftCorner[0], NEW_Y_POS);
	glVertex2f(wiggleTopLeftCorner[0], SCROLLER_Y_HEIGHT);
	glVertex2f(SCROLLER_X_WIDTH, SCROLLER_Y_HEIGHT);
	glVertex2f(SCROLLER_X_WIDTH, NEW_Y_POS);

	// Scroller fill
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(wiggleTopLeftCorner[0], NEW_Y_POS);
	glVertex2f(wiggleTopLeftCorner[0], SCROLLER_Y_HEIGHT);
	glVertex2f(SCROLLER_FILL_X_WIDTH, SCROLLER_Y_HEIGHT);
	glVertex2f(SCROLLER_FILL_X_WIDTH, NEW_Y_POS);

	glEnd();

	// Black outline around the scroller widget
	glPolygonMode(GL_FRONT, GL_LINE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(2.0f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex2f(wiggleTopLeftCorner[0], NEW_Y_POS);
	glVertex2f(wiggleTopLeftCorner[0], SCROLLER_Y_HEIGHT);
	glVertex2f(SCROLLER_X_WIDTH, SCROLLER_Y_HEIGHT);
	glVertex2f(SCROLLER_X_WIDTH, NEW_Y_POS);
	glEnd();

	// Draw the right pointing scroller arrow
	wiggleTopLeftCorner = wiggleTopLeftCorner + Vector2D(AmountScrollerMenuItem::SCROLLER_WIDTH + AmountScrollerMenuItem::INTERIOR_PADDING_ARROWS_SCROLLER, 0.0f);
	this->DrawScrollerArrow(wiggleTopLeftCorner, SCROLLER_HEIGHT, false);

    glPopMatrix();
	glPopAttrib();
	Camera::PopWindowCoords();
}

void AmountScrollerMenuItem::ButtonPressed(const GameControl::ActionButton& pressedButton, 
                                           const GameControl::ActionMagnitude& magnitude) {
    UNUSED_PARAMETER(magnitude);
	this->incrementAmt = this->baseIncrementAmt;

	/*
	// If the user is holding down the shift key then we make the scroller scroll faster
	if ((modifier & KMOD_LSHIFT) == KMOD_LSHIFT || (modifier & KMOD_RSHIFT) == KMOD_RSHIFT) {
		this->incrementAmt = std::max<float>(0.05f * (this->maxValue - this->minValue), 2.0f * this->baseIncrementAmt);
		assert(this->incrementAmt > 0);
	}
	*/

    this->increaseValueButtonPressed = false;
    this->decreaseValueButtonPressed = false;

	switch (pressedButton) {
		case GameControl::LeftButtonAction:
			// Decrease the scroller value...
			this->decreaseValueButtonPressed = true;
			//this->ChangeScrollerValue(-this->incrementAmt);
			break;

		case GameControl::RightButtonAction:
			// Increase the scroller value...
			this->increaseValueButtonPressed = true;
			//this->ChangeScrollerValue(this->incrementAmt);
			break;

		case GameControl::EnterButtonAction:
			this->EscapeFromMenuItem(true);
			break;

        case GameControl::UpButtonAction:
            //this->EscapeFromMenuItem(true);
            //if (this->parent != NULL) {
            //    this->parent->ButtonPressed(GameControl::UpButtonAction);
            //}
            break;

        case GameControl::DownButtonAction:
            //this->EscapeFromMenuItem(true);
            //if (this->parent != NULL) {
            //    this->parent->ButtonPressed(GameControl::DownButtonAction);
            //}
            break;
		case GameControl::EscapeButtonAction:
            this->EscapeFromMenuItem(false);
			break;

		default:
			// Do nothing
			break;
	}
}

void AmountScrollerMenuItem::EscapeFromMenuItem(bool saveChange) {
	
    if (saveChange) {
		// When return is hit, the value is locked and results in a change in the menu item
		// tell the parent about this in order to send out a change event
		if (this->parent != NULL) {
			if (this->currentValue != this->previouslySelectedValue) {
				this->parent->ActivatedMenuItemChanged();
			}
			this->parent->DeactivateSelectedMenuItem();
		}
		if (this->eventHandler != NULL) {
			this->eventHandler->MenuItemConfirmed();
		}
    }
    else {
	    this->ChangeScrollerValue(this->previouslySelectedValue - this->currentValue, false);
	    if (this->parent != NULL) {
		    this->parent->DeactivateSelectedMenuItem();
	    }
	    if (this->eventHandler != NULL) {
		    this->eventHandler->MenuItemCancelled();
	    }
    }
}

void AmountScrollerMenuItem::ButtonReleased(const GameControl::ActionButton& releasedButton) {
    UNUSED_PARAMETER(releasedButton);
    this->decreaseValueButtonPressed = false;
    this->increaseValueButtonPressed = false;
}

void AmountScrollerMenuItem::ChangeScrollerValue(float changeAmt, bool signalEvent) {
	if (fabs(changeAmt) < EPSILON) {
		return;
	}

	float valueBefore = this->currentValue;
	this->currentValue = std::max<float>(this->minValue, std::min<float>(this->maxValue, this->currentValue + changeAmt));

	if (signalEvent && this->alwaysSendChangeUpdates && this->currentValue != valueBefore) {
        if (this->eventHandler != NULL && signalEvent) {
            this->eventHandler->MenuItemScrolled();
        }
    }
}

void AmountScrollerMenuItem::DrawScrollerArrow(const Point2D& topLeftCorner, float arrowHeight, bool isLeftPointing) {
	// Increase the size of the approriate arrow if that button is pressed
	float multiplier = 1.0f;
	if ((this->decreaseValueButtonPressed && isLeftPointing) || (this->increaseValueButtonPressed && !isLeftPointing)) {
		multiplier = 1.2f;
	}
	const float HALF_ARROW_HEIGHT = arrowHeight / 2.0f;
	const float HALF_ARROW_WIDTH  = AmountScrollerMenuItem::SCROLLER_ARROW_WIDTH / 2.0f;

	// Arrow vertices, centered on the origin
	const Point2D APEX_POINT	= (isLeftPointing ? Point2D(-HALF_ARROW_WIDTH, 0.0f) : Point2D(HALF_ARROW_WIDTH, 0.0f));
	const Point2D TOP_POINT		= (isLeftPointing ? Point2D(HALF_ARROW_WIDTH, HALF_ARROW_HEIGHT) : Point2D(-HALF_ARROW_WIDTH, HALF_ARROW_HEIGHT));
	const Point2D BOTTOM_POINT	= (isLeftPointing ? Point2D(HALF_ARROW_WIDTH, -HALF_ARROW_HEIGHT) : Point2D(-HALF_ARROW_WIDTH, -HALF_ARROW_HEIGHT));

	// We set the alpha of the arrows based on the text
	const ColourRGBA& currTextColour = this->currLabel->GetColour();
	
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_POLYGON_BIT);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glPushMatrix();
    glLoadIdentity();
	glTranslatef(topLeftCorner[0] + HALF_ARROW_WIDTH, topLeftCorner[1] - HALF_ARROW_HEIGHT, 0.0f);
	glScalef(multiplier, multiplier, 0.0f);

	// Fill in the arrows
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(1.0f, 0.6f, 0.0f, currTextColour.A());
	
	if (isLeftPointing) {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
		glEnd();
	}
	else {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
		glEnd();
	}
	
	// Draw the outlines of the arrows
	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(2.0f);
	glColor4f(0.0f, 0.0f, 0.0f, currTextColour.A());

	if (isLeftPointing) {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
		glEnd();
	}
	else {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
		glEnd();
	}

	glPopMatrix();
    glPopAttrib();
}


// VerifyMenuItem Functions ***************************************************************************

const float VerifyMenuItem::VERIFY_MENU_HPADDING				= 45.0f;	// Padding on the horizontal border of the verify menu
const float VerifyMenuItem::VERIFY_MENU_VPADDING				= 40.0f;	// Padding on the vertical border of the verify menu
const float VerifyMenuItem::VERIFY_MENU_OPTION_HSPACING	= 50.0f;	// Horizontal padding/spacing between options in verify menu
const float VerifyMenuItem::VERIFY_MENU_OPTION_VSPACING	= 40.0f;	// Vertical padding/spacing between options in verify menu
const float VerifyMenuItem::VERIFY_MENU_OPTION_WOBBLE_FREQ	= 0.2f;	// Frequency of wobble of menu items on selection	

VerifyMenuItem::VerifyMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, 
                               const TextureFontSet* verifyDescFont,
                               const TextureFontSet* verifyIdleFont,
                               const TextureFontSet* verifySelFont) :

GameMenuItem(smLabel, lgLabel, NULL), selectedOption(VerifyMenuItem::Cancel),
descriptionLabel(lgLabel), confirmLabel(smLabel), cancelLabel(smLabel),
verifyMenuWidth(0), verifyMenuHeight(0), verifyMenuActive(false), 
randomMenuBGColour(1,1,1), idleColour(1,1,1), selectionColour(1,0,0),
verifyDescFont(verifyDescFont), verifyIdleFont(verifyIdleFont), verifySelFont(verifySelFont) {

	// Initialize the text for the verify menu to something sensible
	//this->SetVerifyMenuText("Are you sure?", "Yes", "No");

	// Setup the animations to their initial values
	this->optionItemWiggleAnim.SetInterpolantValue(0.0f);
	this->verifyMenuBGScaleAnim.SetInterpolantValue(0.0f);
	this->verifyMenuBGFadeAnim.SetInterpolantValue(0.0f);

    this->descriptionLabel.SetFixedWidth(750);
    this->descriptionLabel.SetAlignment(TextLabel2DFixedWidth::CenterAligned);
}

VerifyMenuItem::~VerifyMenuItem() {
}

/**
 * Sets the currently selected/highlighted verify menu option.
 */
void VerifyMenuItem::SetSelectedVerifyMenuOption(VerifyMenuItem::VerifyMenuOption option) {
	this->selectedOption = option;
	if (option == VerifyMenuItem::Confirm) {
		this->confirmLabel.SetFont(this->verifySelFont);
		this->confirmLabel.SetColour(this->selectionColour);
		this->cancelLabel.SetFont(this->verifyIdleFont);
		this->cancelLabel.SetColour(this->idleColour);
	}
	else {
		this->confirmLabel.SetFont(this->verifyIdleFont);
		this->confirmLabel.SetColour(this->idleColour);
		this->cancelLabel.SetFont(this->verifySelFont);
		this->cancelLabel.SetColour(this->selectionColour);
	}
}

/**
 * Sets the text for the verify menu - this is what will be displayed after the user
 * activates this menu item and is asked to verify their selection.
 */
void VerifyMenuItem::SetVerifyMenuText(const std::string& descriptionText, const std::string& confirmText, const std::string& cancelText) {
	this->descriptionLabel.SetText(descriptionText);
	this->confirmLabel.SetText(confirmText);
	this->cancelLabel.SetText(cancelText);

	// Figure out the maximum width of the verify menu now - to do this we
	// need to set the labels to their largest size
	this->descriptionLabel.SetFont(this->verifyDescFont);
	this->confirmLabel.SetFont(this->verifySelFont);
	this->cancelLabel.SetFont(this->verifySelFont);
	
	// Calculate the width and height - use the maximum possible in each case
    this->verifyMenuWidth = std::max<float>(this->descriptionLabel.GetWidth(), 
		this->confirmLabel.GetLastRasterWidth() + this->cancelLabel.GetLastRasterWidth() + VerifyMenuItem::VERIFY_MENU_OPTION_HSPACING);
	this->verifyMenuWidth += 2 * VerifyMenuItem::VERIFY_MENU_HPADDING;

	this->verifyMenuHeight = this->descriptionLabel.GetHeight() + VerifyMenuItem::VERIFY_MENU_OPTION_VSPACING	+
		std::max<float>(this->confirmLabel.GetHeight(), this->cancelLabel.GetHeight()) + 2 * VerifyMenuItem::VERIFY_MENU_VPADDING;

	// Reset the labels and set the selected option in the menu
	//this->descriptionLabel.SetFont(this->smTextLabel.GetFont());
	this->SetSelectedVerifyMenuOption(this->selectedOption);
}

/**
 * Set the colour for the various texts in the verify menu.
 */
void VerifyMenuItem::SetVerifyMenuColours(const Colour& descTxtColour, const Colour& idleColour, const Colour& selColour) {
	this->descriptionLabel.SetColour(descTxtColour);
	this->idleColour = idleColour;
	this->selectionColour = selColour;
	
	// Set the appropriate colours for the current items
	if (this->selectedOption == VerifyMenuItem::Confirm) {
		this->confirmLabel.SetColour(selColour);
		this->cancelLabel.SetColour(idleColour);
	}
	else {
		this->cancelLabel.SetColour(selColour);
		this->confirmLabel.SetColour(idleColour);	
	}
}

/**
 * Draw the verify menu - this overlays all other menus and sits ontop of all things currently displayed,
 * it allows the user one last chance to cancel their selection (or confirm it).
 */
void VerifyMenuItem::Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight) {
	// Draw default stuff
	GameMenuItem::Draw(dT, topLeftCorner, windowWidth, windowHeight);
	
	// Don't draw the verify menu if it isn't active
	if (!this->verifyMenuActive) {
		return;
	}

	// Draw verify menu stuff
	const Point2D verifyMenuTopLeft((windowWidth - this->verifyMenuWidth) / 2.0f, 
                                    windowHeight - ((windowHeight - this->verifyMenuHeight) / 2.0f));

	const float HALF_VERIFY_MENU_WIDTH	= (this->verifyMenuWidth / 2.0f);
	const float HALF_VERIFY_MENU_HEIGHT	= (this->verifyMenuHeight / 2.0f);
	const float HALF_ANIM_WIDTH	 = this->verifyMenuBGScaleAnim.GetInterpolantValue() * HALF_VERIFY_MENU_WIDTH;
	const float HALF_ANIM_HEIGHT = this->verifyMenuBGScaleAnim.GetInterpolantValue() * HALF_VERIFY_MENU_HEIGHT;

    glPushAttrib(GL_ENABLE_BIT);

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	glPushMatrix();
	glLoadIdentity();
	const float MENU_MIDDLE_X = verifyMenuTopLeft[0] + HALF_VERIFY_MENU_WIDTH;
	const float MENU_MIDDLE_Y = verifyMenuTopLeft[1] - HALF_VERIFY_MENU_HEIGHT;
	glTranslatef(MENU_MIDDLE_X, MENU_MIDDLE_Y, 1.0f);

	// Fill in the background
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(this->randomMenuBGColour.R(), this->randomMenuBGColour.G(), this->randomMenuBGColour.B(), this->verifyMenuBGFadeAnim.GetInterpolantValue());
	GameMenu::DrawBackgroundQuad(HALF_ANIM_WIDTH, HALF_ANIM_HEIGHT);

	// Draw the outline of the background
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(std::min<float>(6.0f, std::max<float>(1.0f, GameDisplay::GetTextScalingFactor() * 6.0f)));
	glColor4f(0.0f, 0.0f, 0.0f, this->verifyMenuBGFadeAnim.GetInterpolantValue());
	GameMenu::DrawBackgroundQuadOutline(HALF_ANIM_WIDTH, HALF_ANIM_HEIGHT);
	
	// Draw points at each corner to get rid of ugly hard edges
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT, GL_POINT);
	glPointSize(std::min<float>(5.0f, std::max<float>(1.0f, GameDisplay::GetTextScalingFactor() * 5.0f)));
	GameMenu::DrawBackgroundQuad(HALF_ANIM_WIDTH, HALF_ANIM_HEIGHT);
	
	glPopMatrix();

	// Draw the menu text...
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(1, 1, 1, 1);

	float wiggleAmtConfirm = 0.0f;
	float wiggleAmtCancel  = 0.0f;
	if (this->selectedOption == VerifyMenuItem::Confirm) {
		wiggleAmtConfirm = this->optionItemWiggleAnim.GetInterpolantValue();	
	}
	else {
		wiggleAmtCancel = this->optionItemWiggleAnim.GetInterpolantValue();	
	}

	ColourRGBA temp;
	// Draw the information/descriptive text for the menu
	const Point2D DESC_TOP_LEFT_CORNER(MENU_MIDDLE_X - this->descriptionLabel.GetWidth()/2.0f, 
	    verifyMenuTopLeft[1] - VerifyMenuItem::VERIFY_MENU_VPADDING); 
	this->descriptionLabel.SetTopLeftCorner(DESC_TOP_LEFT_CORNER);
	
	temp = this->descriptionLabel.GetColour();
	temp[3] = this->verifyMenuBGFadeAnim.GetInterpolantValue();
	this->descriptionLabel.SetColour(temp);

	this->descriptionLabel.Draw();
	
	// Draw the confirm option text
	const Point2D CONFIRM_TOP_LEFT_CORNER(MENU_MIDDLE_X - VerifyMenuItem::VERIFY_MENU_OPTION_HSPACING/2.0f - this->confirmLabel.GetLastRasterWidth(), 
																				DESC_TOP_LEFT_CORNER[1] - VerifyMenuItem::VERIFY_MENU_OPTION_VSPACING - this->descriptionLabel.GetHeight());
	this->confirmLabel.SetTopLeftCorner(CONFIRM_TOP_LEFT_CORNER + Vector2D(0, wiggleAmtConfirm));
	
	temp = this->confirmLabel.GetColour();
	temp[3] = this->verifyMenuBGFadeAnim.GetInterpolantValue();
	this->confirmLabel.SetColour(temp);

	this->confirmLabel.Draw();

	// Draw the cancel option text
	const Point2D CANCEL_TOP_LEFT_CORNER(MENU_MIDDLE_X + VerifyMenuItem::VERIFY_MENU_OPTION_HSPACING/2.0f,
																			 CONFIRM_TOP_LEFT_CORNER[1]);
	this->cancelLabel.SetTopLeftCorner(CANCEL_TOP_LEFT_CORNER + Vector2D(wiggleAmtCancel, 0));
	
	temp = this->cancelLabel.GetColour();
	temp[3] = this->verifyMenuBGFadeAnim.GetInterpolantValue();
	this->cancelLabel.SetColour(temp);	
	
	this->cancelLabel.Draw();

	Camera::PopWindowCoords();

	this->optionItemWiggleAnim.Tick(dT);
	this->verifyMenuBGScaleAnim.Tick(dT);
	this->verifyMenuBGFadeAnim.Tick(dT);

    glPopAttrib();
}

void VerifyMenuItem::ButtonPressed(const GameControl::ActionButton& pressedButton, 
                                   const GameControl::ActionMagnitude& magnitude) {

    UNUSED_PARAMETER(magnitude);

	// If the verify menu is not active then we just exit... this shouldn't happen though
	if (!this->verifyMenuActive) {
		assert(false);
		return;
	}

	switch (pressedButton) {
		case GameControl::LeftButtonAction:
		case GameControl::RightButtonAction:

			// Change the selected item in the verify menu
			if (this->selectedOption == VerifyMenuItem::Cancel) {
				this->SetSelectedVerifyMenuOption(VerifyMenuItem::Confirm);
			}
			else {
				this->SetSelectedVerifyMenuOption(VerifyMenuItem::Cancel);
			}

            if (this->eventHandler != NULL) {
			    this->eventHandler->MenuItemScrolled();
            }

			break;

		case GameControl::EnterButtonAction:
			// Item currently highlighted in the verify menu has just been activated...
			if (this->selectedOption == VerifyMenuItem::Cancel) {
				if (this->parent != NULL) {
					// If the user decided to cancel then just deactivate the item
					this->parent->DeactivateSelectedMenuItem();
				}
				
				if (this->eventHandler != NULL) {
					this->eventHandler->MenuItemCancelled();
				}
			}
			else {
				// If the user decided to confirm then tell the parent that the activated
				// menu item has been verified
				if (this->parent != NULL) {
				    this->parent->DeactivateSelectedMenuItem();
                }

				if (this->eventHandler != NULL) {
					this->eventHandler->MenuItemConfirmed();
				}
			}
			break;

		case GameControl::EscapeButtonAction:
			// User has indicated that they aren't sure... Set the selected item to cancel,
			// if the item is already set to cancel then just cancel the verify menu
			if (this->selectedOption == VerifyMenuItem::Cancel) {
				if (this->parent != NULL) {
					this->parent->DeactivateSelectedMenuItem();
				}
				if (this->eventHandler != NULL) {
					this->eventHandler->MenuItemCancelled();
				}
			}
			else {
				this->SetSelectedVerifyMenuOption(VerifyMenuItem::Cancel);

                if (this->eventHandler != NULL) {
			        this->eventHandler->MenuItemScrolled();
                }
			}
			break;

		default:
			break;
	}
}

void VerifyMenuItem::Activate() {
    GameMenuItem::Activate();

	// Assign a random colour for the background of the verify menu
	const int randomColourIdx = (Randomizer::GetInstance()->RandomUnsignedInt() % GameMenu::NUM_RAND_COLOURS);
	this->randomMenuBGColour = GameMenu::RAND_COLOUR_LIST[randomColourIdx];

	// Setup any animations that occur within the verify menu
	float amplitude = std::min<float>(VerifyMenuItem::VERIFY_MENU_OPTION_HSPACING, VerifyMenuItem::VERIFY_MENU_OPTION_VSPACING) / 4.0f;
	std::vector<float> wiggleValues;
	wiggleValues.reserve(5);
	wiggleValues.push_back(0.0f);
	wiggleValues.push_back(amplitude);
	wiggleValues.push_back(0.0f);
	wiggleValues.push_back(-amplitude);
	wiggleValues.push_back(0.0f);

	std::vector<double> timeValues;
	timeValues.reserve(5);
	timeValues.push_back(0.0);
	timeValues.push_back(VerifyMenuItem::VERIFY_MENU_OPTION_WOBBLE_FREQ);
	timeValues.push_back(2*VerifyMenuItem::VERIFY_MENU_OPTION_WOBBLE_FREQ);
	timeValues.push_back(3*VerifyMenuItem::VERIFY_MENU_OPTION_WOBBLE_FREQ);
	timeValues.push_back(4*VerifyMenuItem::VERIFY_MENU_OPTION_WOBBLE_FREQ);

	this->optionItemWiggleAnim.SetLerp(timeValues, wiggleValues);
	this->optionItemWiggleAnim.SetRepeat(true);

	// When the verify menu opens it has a 'ghost' that keeps opening but fades out
	std::vector<double> openTimeVals;
	openTimeVals.reserve(3);
	openTimeVals.push_back(0.0);
	openTimeVals.push_back(0.4);
	
	std::vector<float> bgScaleAmts;
	bgScaleAmts.reserve(2);
	bgScaleAmts.push_back(EPSILON);
	bgScaleAmts.push_back(1.0f);

	std::vector<float> bgFadeAmts;
	bgFadeAmts.reserve(2);
	bgFadeAmts.push_back(0.0f);
	bgFadeAmts.push_back(1.0f);

	this->verifyMenuBGFadeAnim.SetLerp(openTimeVals, bgFadeAmts);
	this->verifyMenuBGScaleAnim.SetLerp(openTimeVals, bgScaleAmts);
	this->verifyMenuBGFadeAnim.SetRepeat(false);
	this->verifyMenuBGScaleAnim.SetRepeat(false);

	// Activate the menu
	this->verifyMenuActive = true;
}

void VerifyMenuItem::Deactivate() {
    GameMenuItem::Deactivate();

	// Deactivate the menu
	this->verifyMenuActive = false;

	// Turn off any animations that occur within the verify menu
	this->optionItemWiggleAnim.ClearLerp();
	this->optionItemWiggleAnim.SetRepeat(false);
	this->optionItemWiggleAnim.SetInterpolantValue(0.0f);

	this->verifyMenuBGFadeAnim.ClearLerp();
	this->verifyMenuBGFadeAnim.SetInterpolantValue(0.0f);

	this->verifyMenuBGScaleAnim.ClearLerp();
	this->verifyMenuBGScaleAnim.SetInterpolantValue(0.0f);
}