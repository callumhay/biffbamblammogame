/**
 * RandomItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "RandomItem.h"
#include "GameItemFactory.h"

const char*  RandomItem::RANDOM_ITEM_NAME = "Random";
const long RandomItem::TIME_BETWEEN_BLINKS_IN_MS = 100;

RandomItem::RandomItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(RandomItem::RANDOM_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral), randomItem(NULL),
currRandomIdx(-1), lastBlinkTime(0) {

	// Build a list of all possible random items, but make sure the random item type isn't included in there...
    this->possibleItemDropTypes = gameModel->GetCurrentLevel()->GetAllowableItemDropTypes();
    for (std::vector<GameItem::ItemType>::iterator iter = this->possibleItemDropTypes.begin();
         iter != this->possibleItemDropTypes.end(); ++iter) {

        if (*iter == GameItem::RandomItem) {
            this->possibleItemDropTypes.erase(iter);
            break;
        }
    }

    this->currRandomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(this->possibleItemDropTypes.size());
    this->lastBlinkTime = BlammoTime::GetSystemTimeInMillisecs();
}

RandomItem::~RandomItem() {
    if (this->randomItem != NULL) {
	    delete this->randomItem;
	    this->randomItem = NULL;
    }
}

double RandomItem::Activate() {

    // Choose the random item...
    assert(this->randomItem == NULL);
    GameItem::ItemType randomItemType = this->possibleItemDropTypes[currRandomIdx];
    this->randomItem = GameItemFactory::GetInstance()->CreateItem(randomItemType, this->GetCenter(), gameModel);
    assert(this->randomItem != NULL);
    assert(this->randomItem->GetItemType() != GameItem::RandomItem);

    // Activate it!
    this->isActive = true;
    this->SetItemDisposition(this->randomItem->GetItemDisposition());
    this->SetItemName(this->randomItem->GetName());
    GameEventManager::Instance()->ActionRandomItemActivated(*this, *this->randomItem);
    return this->randomItem->Activate();
}

GameItem::ItemType RandomItem::GetItemType() const {
    if (this->isActive) {
        assert(this->randomItem != NULL);
        return this->randomItem->GetItemType();
    }
    return GameItem::RandomItem;
}

GameItem::ItemType RandomItem::GetBlinkingRandomItemType() const {
    unsigned long currTime = BlammoTime::GetSystemTimeInMillisecs();
    if (labs(static_cast<long>(currTime) - static_cast<long>(this->lastBlinkTime)) >= TIME_BETWEEN_BLINKS_IN_MS) {
        // Choose a new random item to show...
        this->currRandomIdx = (this->currRandomIdx + 1 + (Randomizer::GetInstance()->RandomUnsignedInt() % 
            static_cast<int>(this->possibleItemDropTypes.size())-1)) % static_cast<int>(this->possibleItemDropTypes.size());
        this->lastBlinkTime = currTime;
    }

    return this->possibleItemDropTypes[this->currRandomIdx];
}