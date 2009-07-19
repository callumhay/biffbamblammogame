/**
 * GameMenuItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameMenuItem.h"
#include "GameMenu.h"

#include "../BlammoEngine/Camera.h"

const float GameMenuItem::MENU_ITEM_WOBBLE_AMT_LARGE	= 8.0f;
const float GameMenuItem::MENU_ITEM_WOBBLE_AMT_SMALL	= 4.0f;
const float GameMenuItem::MENU_ITEM_WOBBLE_FREQ				= 0.2f;
const float GameMenuItem::SUB_MENU_PADDING						= 15.0f;

// GameMenuItem Functions *******************************************

GameMenuItem::GameMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel, GameSubMenu* subMenu) : 
smTextLabel(smLabel), lgTextLabel(lgLabel), subMenu(subMenu) {
	
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

float GameMenuItem::GetWidth() const {
	return this->currLabel->GetLastRasterWidth();
}

/**
 * Draw this menu item on the screen with the given coordinate
 * point specifying the top-left corner location where it will
 * be drawn, in window coordinates.
 */
void GameMenuItem::Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight) {
	float wiggleAmount = this->wiggleAnimation.GetInterpolantValue();

	this->currLabel->SetTopLeftCorner(topLeftCorner + Vector2D(wiggleAmount, 0.0f));
	this->currLabel->Draw();

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
	// Obtain the latest interpolated wiggle value
	float wiggleAmount = this->wiggleAnimation.GetInterpolantValue();
	
	// Animate the wiggle and/or pulse (if there are any animations loaded)
	this->wiggleAnimation.Tick(dT);

	// Draw the base label portion of the item
	Point2D wiggleTopLeftCorner = topLeftCorner + Vector2D(wiggleAmount, 0.0f);
	this->currLabel->SetText(this->baseLabelStr);
	this->currLabel->SetTopLeftCorner(wiggleTopLeftCorner);
	this->currLabel->Draw();

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
	const std::string CURR_SEL_ITEM_STR = this->selectionList[this->selectedIndex];
	this->currLabel->SetText(CURR_SEL_ITEM_STR);
	this->currLabel->SetTopLeftCorner(wiggleTopLeftCorner);
	this->currLabel->Draw();

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
	const Point2D APEX_POINT		= (isLeftPointing ? Point2D(-HALF_ARROW_WIDTH, 0.0f) : Point2D(HALF_ARROW_WIDTH, 0.0f));
	const Point2D TOP_POINT			= (isLeftPointing ? Point2D(HALF_ARROW_WIDTH, HALF_ARROW_HEIGHT) : Point2D(-HALF_ARROW_WIDTH, HALF_ARROW_HEIGHT));
	const Point2D BOTTOM_POINT	= (isLeftPointing ? Point2D(HALF_ARROW_WIDTH, -HALF_ARROW_HEIGHT) : Point2D(-HALF_ARROW_WIDTH, -HALF_ARROW_HEIGHT));

	// We set the alpha of the arrows based on the text
	const ColourRGBA currTextColour = this->currLabel->GetColour();

	Camera::PushWindowCoords();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw the outlines of the arrows
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(2.0f);
	glColor4f(0.0f, 0.0f, 0.0f, currTextColour.A());
	
	glPushMatrix();
	glTranslatef(topLeftCorner[0] + HALF_ARROW_WIDTH, topLeftCorner[1] - HALF_ARROW_HEIGHT, 0.0f);

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
	
	glPopMatrix();
	glDisable(GL_BLEND);
	Camera::PopWindowCoords();
}

/**
 * Called from the parent menu of this item when a key is pressed and this item
 * is the one currently selected and relevant.
 */
void SelectionListMenuItem::KeyPressed(GameMenu* parent, SDLKey key) {
	assert(parent != NULL);

	// Key pressing does nothing if there's nothing to select from
	if (this->selectionList.size() == 0) {
		return;
	}

	switch (key) {
		case SDLK_LEFT:
			// Move the selection down one item
			this->selectedIndex--;
			if (this->selectedIndex < 0) {
				this->selectedIndex = this->selectionList.size() - 1;
			}
			break;

		case SDLK_RIGHT:
			// Move the selection up one item
			this->selectedIndex = (this->selectedIndex + 1) % this->selectionList.size();
			break;

		case SDLK_RETURN:
			// When return is hit, the selection is locked and results in a change in the menu item
			// tell the parent about this in order to send out a change event
			if (this->selectedIndex != this->previouslySelectedIndex) {
				parent->SelectedMenuItemChanged();
			}
			parent->DeactivateSelectedMenuItem();
			break;

		case SDLK_ESCAPE:
			// If the user hits ESC then we deselect the currently selected item (i.e., this one)
			// and we make sure the selection inside the item doesn't change
			this->selectedIndex = this->previouslySelectedIndex;
			parent->DeactivateSelectedMenuItem();
			break;

		default:
			break;
	}

	assert(this->selectedIndex >= 0 && this->selectedIndex < static_cast<int>(this->selectionList.size()));
}

/**
 * Tell this menu item to become selected. This function must be called
 * in order to ensure this item is properly activated.
 */
void SelectionListMenuItem::Activate() {
	// Set the previous index to the current one (so we keep track of it)
	this->previouslySelectedIndex = this->selectedIndex;
}

// VerifyMenuItem Functions ***************************************************************************

const float VerifyMenuItem::VERIFY_MENU_PADDING					= 15.0f;	// Padding along the border of the verify menu
const float VerifyMenuItem::VERIFY_MENU_OPTION_HSPACING	= 10.0f;	// Horizontal padding/spacing between options in verify menu
const float VerifyMenuItem::VERIFY_MENU_OPTION_VSPACING	= 10.0f;	// Vertical padding/spacing between options in verify menu

VerifyMenuItem::VerifyMenuItem(const TextLabel2D& smLabel, const TextLabel2D& lgLabel) :
GameMenuItem(smLabel, lgLabel, NULL), selectedOption(VerifyMenuItem::Confirm),
descriptionLabel(smLabel), confirmLabel(smLabel), cancelLabel(smLabel),
verifyMenuWidth(0), verifyMenuHeight(0) {

	// Initialize the text for the verify menu to something sensible
	this->SetVerifyMenuText("Are you sure?", "Yes", "No");
}

VerifyMenuItem::~VerifyMenuItem() {
}

/**
 * Sets the currently selected/highlighted verify menu option.
 */
void VerifyMenuItem::SetSelectedVerifyMenuOption(VerifyMenuItem::VerifyMenuOption option) {
	this->selectedOption = option;
	if (option == VerifyMenuItem::Confirm) {
		this->confirmLabel.SetFont(this->lgTextLabel.GetFont());
		this->cancelLabel.SetFont(this->smTextLabel.GetFont());
	}
	else {
		this->confirmLabel.SetFont(this->smTextLabel.GetFont());
		this->cancelLabel.SetFont(this->lgTextLabel.GetFont());			
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
	this->descriptionLabel.SetFont(this->lgTextLabel.GetFont());
	this->confirmLabel.SetFont(this->lgTextLabel.GetFont());
	this->cancelLabel.SetFont(this->lgTextLabel.GetFont());
	
	// Calculate the width and height - use the maximum possible in each case
	this->verifyMenuWidth = std::max<float>(this->descriptionLabel.GetLastRasterWidth(), 
		this->confirmLabel.GetLastRasterWidth() + this->cancelLabel.GetLastRasterWidth() + VerifyMenuItem::VERIFY_MENU_OPTION_HSPACING);
	this->verifyMenuWidth += 2 * VerifyMenuItem::VERIFY_MENU_PADDING;

	this->verifyMenuHeight = this->descriptionLabel.GetHeight() + VerifyMenuItem::VERIFY_MENU_OPTION_VSPACING	+
		std::max<float>(this->confirmLabel.GetHeight(), this->cancelLabel.GetHeight()) + 2 * VerifyMenuItem::VERIFY_MENU_PADDING;

	// Reset the labels and set the selected option in the menu
	this->descriptionLabel.SetFont(this->smTextLabel.GetFont());
	this->SetSelectedVerifyMenuOption(this->selectedOption);
}

/**
 * Draw the verify menu - this overlays all other menus and sits ontop of all things currently displayed,
 * it allows the user one last chance to cancel their selection (or confirm it).
 */
void VerifyMenuItem::Draw(double dT, const Point2D& topLeftCorner, int windowWidth, int windowHeight) {
	const Point2D verifyMenuTopLeft((windowWidth - this->verifyMenuWidth) / 2.0f, 
																	windowHeight - ((windowHeight - this->verifyMenuHeight) / 2.0f));

	const float HALF_VERIFY_MENU_WIDTH	= this->verifyMenuWidth / 2.0f;
	const float HALF_VERIFY_MENU_HEIGHT	= this->verifyMenuHeight / 2.0f;

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(verifyMenuTopLeft[0] + HALF_VERIFY_MENU_WIDTH, verifyMenuTopLeft[1] - HALF_VERIFY_MENU_HEIGHT, 0.0f);

	GameMenu::DrawBackgroundQuad(HALF_VERIFY_MENU_WIDTH, HALF_VERIFY_MENU_HEIGHT);
	
	glPopMatrix();
}

void VerifyMenuItem::KeyPressed(GameMenu* parent, SDLKey key) {
	switch (key) {
		case SDLK_LEFT:
		case SDLK_RIGHT:
			// Change the selected item in the verify menu
			// TODO
			break;

		case SDLK_RETURN:
			// Item currently highlighted in the verify menu has just been activated...
			// TODO
			break;

		case SDLK_ESCAPE:
			// User has indicated that they aren't sure...
			// TODO
			break;

		default:
			break;
	}
}