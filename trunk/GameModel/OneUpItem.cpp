/**
 * OneUpItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "OneUpItem.h"

const char* OneUpItem::ONE_UP_ITEM_NAME	      = "1Up";
const double OneUpItem::ONE_UP_TIMER_IN_SECS	= 0.0;

OneUpItem::OneUpItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(OneUpItem::ONE_UP_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

OneUpItem::~OneUpItem() {
}

double OneUpItem::Activate() {
	this->isActive = true;

	int numLivesLeft = this->gameModel->GetLivesLeft();
	assert(numLivesLeft >= 0);
	numLivesLeft++;
	this->gameModel->SetLivesLeft(numLivesLeft);

	GameItem::Activate();
	return OneUpItem::ONE_UP_TIMER_IN_SECS;
}

void OneUpItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	this->isActive = false;
	GameItem::Deactivate();
}