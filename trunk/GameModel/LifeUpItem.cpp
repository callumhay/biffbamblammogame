/**
 * LifeUpItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LifeUpItem.h"

const char* LifeUpItem::LIFE_UP_ITEM_NAME	    = "LifeUp";
const double LifeUpItem::LIFE_UP_TIMER_IN_SECS	= 0.0;

LifeUpItem::LifeUpItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(LifeUpItem::LIFE_UP_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

LifeUpItem::~LifeUpItem() {
}

double LifeUpItem::Activate() {
	this->isActive = true;

	int numLivesLeft = this->gameModel->GetLivesLeft();
	assert(numLivesLeft >= 0);
	numLivesLeft++;
	this->gameModel->SetLivesLeft(numLivesLeft);

	GameItem::Activate();
	return LifeUpItem::LIFE_UP_TIMER_IN_SECS;
}

void LifeUpItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	this->isActive = false;
	GameItem::Deactivate();
}