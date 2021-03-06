/**
 * Blammopedia.cpp
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

#include "Blammopedia.h"
#include "ResourceManager.h"

#include "BlammoEngine/StringHelper.h"
#include "BlammoEngine/Texture2D.h"
#include "GameModel/GameItemFactory.h"
#include "GameView/GameViewConstants.h"

// Item, block and status effect file keywords/syntax constants
const char* Blammopedia::Entry::NAME_KEYWORD				= "Name:";
const char* Blammopedia::Entry::ITEM_TEXTURE_KEYWORD		= "ItemTexture:";
const char* Blammopedia::Entry::HUD_OUTLINE_TEXTURE_KEYWORD	= "HUDOutlineTexture:";
const char* Blammopedia::Entry::HUD_FILL_TEXTURE_KEYWORD	= "HUDFillTexture:";
const char* Blammopedia::Entry::DESCRIPTION_KEYWORD			= "Desc:";
const char* Blammopedia::Entry::FINEPRINT_KEYWORD           = "FinePrint:";
const char* Blammopedia::Entry::DISPLAY_TEXTURE_KEYWORD     = "DisplayTexture:";

Blammopedia::Blammopedia() : lockedItemTexture(NULL) {
	static const std::string BLAMMOPEDIA_ITEMS_DIR  = ResourceManager::GetBlammopediaResourceDir()  + std::string("items/");
	static const std::string BLAMMOPEDIA_BLOCKS_DIR = ResourceManager::GetBlammopediaResourceDir()  + std::string("blocks/");
	static const std::string BLAMMOPEDIA_MISC_DIR   = ResourceManager::GetBlammopediaResourceDir()  + std::string("misc/");

	// Initialize all of the various types of entries in the blammopedia...

	// Item Entry Types...
    this->itemEntries.insert(std::make_pair(GameItem::BallSpeedUpItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("fast_ball.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::BallSlowDownItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("slow_ball.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::UberBallItem,				new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("uber_ball.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::InvisiBallItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("invisi_ball.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::GhostBallItem,            new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("ghost_ball.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::LaserBulletPaddleItem,	new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("laser_gun.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::MultiBall3Item,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("multi_ball_3.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::MultiBall5Item,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("multi_ball_5.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::PaddleGrowItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("paddle_grow.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::PaddleShrinkItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("paddle_shrink.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::BallShrinkItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("ball_shrink.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::BallGrowItem,				new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("ball_grow.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::BlackoutItem,				new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("blackout.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::UpsideDownItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("flip.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::BallSafetyNetItem,		new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("safety_net.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::LifeUpItem,				new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("life_up.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::PoisonPaddleItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("poison.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::StickyPaddleItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("sticky_paddle.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::PaddleCamItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("paddle_camera.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::BallCamItem,				new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("ball_camera.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::LaserBeamPaddleItem,		new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("laser_beam.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::GravityBallItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("gravity_ball.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::RocketPaddleItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("rocket.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::CrazyBallItem,			new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("crazy_ball.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::ShieldPaddleItem,         new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("shield.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::FireBallItem,				new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("fire_ball.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::IceBallItem,				new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("ice_ball.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::OmniLaserBallItem,        new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("omni_laser_ball.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::InvisiPaddleItem,         new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("invisi_paddle.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::MagnetPaddleItem,         new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("paddle_magnet.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::MineLauncherPaddleItem,   new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("paddle_mine.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::RemoteCtrlRocketItem,     new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("remote_control_rocket.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::FlameBlasterPaddleItem,   new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("flameblaster_paddle.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::IceBlasterPaddleItem,     new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("iceblaster_paddle.txt"))));
    this->itemEntries.insert(std::make_pair(GameItem::RandomItem,				new ItemEntry(BLAMMOPEDIA_ITEMS_DIR + std::string("random.txt"))));
    
	// Block Entry Types...
    this->blockEntries.insert(std::make_pair(LevelPiece::Solid,             new SolidBlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("solid_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::SolidTriangle,     new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("solid_triangle_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::Breakable,         new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("breakable_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::BreakableTriangle, new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("breakable_triangle_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::Bomb,              new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("bomb_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::Ink,               new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("ink_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::Prism,             new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("prism_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::PrismTriangle,     new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("prism_triangle_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::Portal,            new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("portal_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::Cannon,            new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("cannon_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::FragileCannon,     new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("fragile_cannon_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::Collateral,        new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("collateral_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::Tesla,             new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("tesla_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::ItemDrop,          new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("item_drop_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::Switch,            new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("switch_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::OneWay,            new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("one_way_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::NoEntry,           new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("no_entry_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::LaserTurret,       new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("laser_turret_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::RocketTurret,      new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("rocket_turret_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::MineTurret,        new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("mine_turret_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::AlwaysDrop,        new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("always_drop_block.txt"))));
    this->blockEntries.insert(std::make_pair(LevelPiece::Regen,             new BlockEntry(BLAMMOPEDIA_BLOCKS_DIR + std::string("regen_block.txt"))));
}

Blammopedia::~Blammopedia() {
	// Clean up the locked item texture
	if (this->lockedItemTexture != NULL) {
		bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->lockedItemTexture);
        UNUSED_VARIABLE(success);
		assert(success);
	}

	// Clear all of the entries in the blammopedia
	for (std::map<GameItem::ItemType, ItemEntry*>::iterator iter = this->itemEntries.begin(); iter != this->itemEntries.end(); ++iter) {
		ItemEntry* itemEntry = iter->second;
		delete itemEntry;
		itemEntry = NULL;
	}
	this->itemEntries.clear();
	
    for (std::map<LevelPiece::LevelPieceType, AbstractBlockEntry*>::iterator iter = this->blockEntries.begin(); iter != this->blockEntries.end(); ++iter) {
		AbstractBlockEntry* blockEntry = iter->second;
		delete blockEntry;
		blockEntry = NULL;
	}
	this->blockEntries.clear();
	
    for (std::map<LevelPiece::PieceStatus, MiscEntry*>::iterator iter = this->miscEntries.begin();
			 iter != this->miscEntries.end(); ++iter) {

		MiscEntry* statusEntry = iter->second;
		delete statusEntry;
		statusEntry = NULL;
	}
	this->miscEntries.clear();
}

bool Blammopedia::HasUnviewed() const {
    return (this->HasUnviewedBlocks() || this->HasUnviewedItems());
}

bool Blammopedia::HasUnviewedBlocks() const {
    for (BlockEntryMapConstIter iter = this->blockEntries.begin(); iter != this->blockEntries.end(); ++iter) {
        if (!iter->second->GetIsLocked() && !iter->second->GetHasBeenViewed()) {
            return true;
        }
    }
    return false;
}

bool Blammopedia::HasUnviewedItems() const {
    for (ItemEntryMapConstIter iter = this->itemEntries.begin(); iter != this->itemEntries.end(); ++iter) {
        if (!iter->second->GetIsLocked() && !iter->second->GetHasBeenViewed()) {
            return true;
        }
    }
    return false;
}

void Blammopedia::ClearProgressData() {
    // Clear the blammopedia of all progress data...
    for (BlockEntryMapIter iter = this->blockEntries.begin(); iter != blockEntries.end(); ++iter) {
        AbstractBlockEntry* blockEntry = iter->second;
        blockEntry->SetIsLocked(true);
    }
    for (ItemEntryMapIter iter = this->itemEntries.begin(); iter != this->itemEntries.end(); ++iter) {
        ItemEntry* itemEntry = iter->second;
        itemEntry->SetIsLocked(true);
    }
}

Blammopedia* Blammopedia::BuildFromBlammopediaFile(const std::string &filepath) {
    bool success = true;
    
    std::map<GameItem::ItemType, std::pair<bool, bool> > itemStatusMap;
    std::map<LevelPiece::LevelPieceType, std::pair<bool, bool> > blockStatusMap;

	// Start by reading in the unlock file which tells us which blammopedia entries are locked/unlocked...
    std::ifstream inFile(filepath.c_str(), std::ifstream::in | std::ifstream::binary);
	if (inFile.is_open()) {

        success &= Blammopedia::ReadItemEntires(inFile, itemStatusMap);
		success &= Blammopedia::ReadBlockEntries(inFile, blockStatusMap);

        inFile.close();
    }
    else {
        success = false;
    }

	Blammopedia* blammopedia = new Blammopedia();
    blammopedia->blammopediaFile = filepath;

	// If we're unsuccessful in reading the blammopedia file then we just write an 'empty' 
	// (i.e., all entries are locked) blammopedia file from scratch
	if (!success) {
		success = blammopedia->WriteAsEntryStatusFile();
		if (!success) {
			assert(false);
			debug_output("Critical error: could not write blammopedia file.");
			delete blammopedia;
			blammopedia = NULL;
			return NULL;
		}
	}
	
	// Set the lock/unlocked status for all entries and read each entry from its respective file...
    for (std::map<GameItem::ItemType, std::pair<bool, bool> >::const_iterator iter = itemStatusMap.begin();
         iter != itemStatusMap.end(); ++iter) {

		ItemEntry* itemEntry = blammopedia->GetItemEntry(iter->first);
		assert(itemEntry != NULL);
        itemEntry->SetIsLocked(iter->second.first);
        itemEntry->SetHasBeenViewed(iter->second.second);
    }
	for (std::map<LevelPiece::LevelPieceType, std::pair<bool, bool> >::const_iterator iter = blockStatusMap.begin();
         iter != blockStatusMap.end(); ++iter) {

		AbstractBlockEntry* blockEntry = blammopedia->GetBlockEntry(iter->first);
		assert(blockEntry != NULL);
        blockEntry->SetIsLocked(iter->second.first);
        blockEntry->SetHasBeenViewed(iter->second.second);
	}

	//for (std::map<LevelPiece::PieceStatus, bool>::const_iterator iter = statusEffectStatusMap.begin(); iter != statusEffectStatusMap.end(); ++iter) {
	//	MiscEntry* statusEntry = blammopedia->GetMiscEntry(iter->first);
	//	assert(statusEntry != NULL);
	//	statusEntry->SetIsLocked(iter->second);
	//}
	
	// Go through all entries in the blammopedia an initialize them
	success = blammopedia->InitializeEntries();
	if (!success) {
		assert(false);
		delete blammopedia;
		blammopedia = NULL;
	}

	return blammopedia;
}

// Writes the current state of this blammopedia object to a entry status file (the file that stores whether
// entries are locked/unlocked).
bool Blammopedia::WriteAsEntryStatusFile() const {
	// Open or create the blammopedia file for writing
    std::ofstream outFile(this->blammopediaFile.c_str(), std::ios::out | std::ios::binary);
	if (!outFile.is_open()) {
		// Couldn't open for writing...
		outFile.close();
		return false;
	}

	// Write all of the item entry's lock statuses first...
    size_t itemOutDataSize = 3*this->itemEntries.size() + 1;
    char* itemOutData = new char[itemOutDataSize];
    
    assert(this->itemEntries.size() <= CHAR_MAX);
    itemOutData[0] = this->itemEntries.size();
    int count = 1;
    for (ItemEntryMapConstIter iter = this->itemEntries.begin(); iter != this->itemEntries.end(); ++iter) {
        int itemType = iter->first;
        ItemEntry* itemEntry = iter->second;
        assert(itemEntry != NULL);
        assert(itemType <= CHAR_MAX);
        itemOutData[count++] = itemType;
        itemOutData[count++] = itemEntry->GetIsLocked() ? 1 : 0;
        itemOutData[count++] = itemEntry->GetHasBeenViewed() ? 1 : 0;
    }
    outFile.write(itemOutData, itemOutDataSize);
    delete[] itemOutData;
    itemOutData = NULL;

	// Same for blocks...
    size_t blockOutDataSize = 3*this->blockEntries.size() + 1;
    char* blockOutData = new char[blockOutDataSize];
    assert(this->blockEntries.size() <= CHAR_MAX);
    blockOutData[0] = this->blockEntries.size();
    count = 1;
    for (BlockEntryMapConstIter iter = this->blockEntries.begin(); iter != this->blockEntries.end(); ++iter) {
        const AbstractBlockEntry* blockEntry = iter->second;
        const LevelPiece::LevelPieceType& pieceType = iter->first;
		assert(blockEntry != NULL);
        assert(pieceType <= CHAR_MAX);
        blockOutData[count++] = pieceType;
        blockOutData[count++] = blockEntry->GetIsLocked() ? 1 : 0;
        blockOutData[count++] = blockEntry->GetHasBeenViewed() ? 1 : 0;
    }
    outFile.write(blockOutData, blockOutDataSize);
    delete[] blockOutData;
    blockOutData = NULL;
    
    outFile.close();
	return true;
}

// Goes through every entry in the blammopedia and initializes it from file - this includes loading
// the textures of various entries, etc.
bool Blammopedia::InitializeEntries() {

	bool success = true;
	bool allSuccess = true;

	// Load the item entries
	for (std::map<GameItem::ItemType, ItemEntry*>::iterator iter = this->itemEntries.begin(); iter != this->itemEntries.end(); ++iter) {
		ItemEntry* itemEntry = iter->second;
		success = itemEntry->PopulateFromFile();
		if (!success) { allSuccess = false; }
	}

	// Load the locked item texture...
	assert(this->lockedItemTexture == NULL);
	this->lockedItemTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
		GameViewConstants::GetInstance()->TEXTURE_LOCKED_BLAMMOPEDIA_ENTRY, Texture::Trilinear, GL_TEXTURE_2D));
	if (this->lockedItemTexture == NULL) {
		allSuccess = false;
	}

    // Load level piece (block) entries
	for (std::map<LevelPiece::LevelPieceType,
		AbstractBlockEntry*>::iterator iter = this->blockEntries.begin(); iter != this->blockEntries.end(); ++iter) {
		AbstractBlockEntry* blockEntry = iter->second;
		success = blockEntry->PopulateFromFile();
		if (!success) { allSuccess = false; }
	}

	for (std::map<LevelPiece::PieceStatus, MiscEntry*>::iterator iter = this->miscEntries.begin();
			 iter != this->miscEntries.end(); ++iter) {

		MiscEntry* miscEntry = iter->second;
		success = miscEntry->PopulateFromFile();
		if (!success) { allSuccess = false; }
	}

	return allSuccess;
}

bool Blammopedia::ReadItemEntires(std::istream& inStream, std::map<GameItem::ItemType, std::pair<bool, bool> >& itemStatusMap) {
    char tempReadChar;
    if (!inStream.read(&tempReadChar, 1)) {
	    debug_output("Error while reading item entries from blammopedia file.");
	    return false; 
    }

    int numItems = static_cast<int>(tempReadChar);
    for (int i = 0; i < numItems; i++) {

        // Read the item type enumeration
        if (!inStream.read(&tempReadChar, 1)) {
	        debug_output("Error while reading item type for item entry from blammopedia file.");
	        return false;
        }
        int itemReadType = static_cast<int>(tempReadChar);
        if (!GameItemFactory::GetInstance()->IsValidItemType(itemReadType)) {
		    debug_output("Error while reading blammopedia file invalid item enumeration type found while trying to read item type.");
		    return false;
        }
        GameItem::ItemType itemType = static_cast<GameItem::ItemType>(itemReadType);

        // Read whether the item is locked or not
        if (!inStream.read(&tempReadChar, 1)) {
	        debug_output("Error while reading blammopedia file could not read locked status for item.");
	        return false;
        }
        int isLockedInt = static_cast<int>(tempReadChar);
        bool isLockedBool = (isLockedInt == 1) ? true : false;
        
        if (!inStream.read(&tempReadChar, 1)) {
	        debug_output("Error while reading blammopedia file could not read viewed status for item.");
	        return false;
        }
        int isViewedInt = static_cast<int>(tempReadChar);
        bool hasBeenViewed = (isViewedInt == 1) ? true : false;

        std::pair<std::map<GameItem::ItemType, std::pair<bool, bool> >::iterator, bool> insertResult = 
            itemStatusMap.insert(std::make_pair(itemType, std::make_pair(isLockedBool, hasBeenViewed)));
	    if (!insertResult.second) {
		    debug_output("Error while reading blammopedia file duplicate item type found: " << itemReadType);
		    return false;
	    }
    }
    
    return true;
}

bool Blammopedia::ReadBlockEntries(std::istream& inStream, std::map<LevelPiece::LevelPieceType, std::pair<bool, bool> >& blockStatusMap) {
    char tempReadChar;
    if (!inStream.read(&tempReadChar, 1)) {
	    debug_output("Error while reading block entries from blammopedia file.");
	    return false; 
    }

    int numBlocks = static_cast<int>(tempReadChar);
    for (int i = 0; i < numBlocks; i++) {

        // Read the item type enumeration
        if (!inStream.read(&tempReadChar, 1)) {
	        debug_output("Error while reading item type for item entry from blammopedia file.");
	        return false;
        }
        int levelPieceReadType = static_cast<int>(tempReadChar);
        if (!LevelPiece::IsValidLevelPieceType(levelPieceReadType)) {
		    debug_output("Error while reading blammopedia file invalid level piece enumeration type found while trying to read block type.");
		    return false;
        }
        LevelPiece::LevelPieceType levelPieceType = static_cast<LevelPiece::LevelPieceType>(levelPieceReadType);

        // Read whether the item is locked or not
        if (!inStream.read(&tempReadChar, 1)) {
	        debug_output("Error while reading blammopedia file could not read locked status for block.");
	        return false;
        }
        int isLockedInt = static_cast<int>(tempReadChar);
        bool isLockedBool = (isLockedInt == 1) ? true : false;
        
        if (!inStream.read(&tempReadChar, 1)) {
	        debug_output("Error while reading blammopedia file could not read viewed status for item.");
	        return false;
        }
        int isViewedInt = static_cast<int>(tempReadChar);
        bool hasBeenViewed = (isViewedInt == 1) ? true : false;

        std::pair<std::map<LevelPiece::LevelPieceType, std::pair<bool, bool> >::iterator, bool> insertResult = 
            blockStatusMap.insert(std::make_pair(levelPieceType, std::make_pair(isLockedBool, hasBeenViewed)));
	    if (!insertResult.second) {
		    debug_output("Error while reading blammopedia file duplicate level piece type found: " << levelPieceReadType);
		    return false;
	    }
    }
    
    return true;
}

// Populates basic entry values from the given stream
bool Blammopedia::Entry::PopulateBaseValuesFromStream(std::istream& inStream) {
	bool success = true;
	std::string tempReadStr;
	while (success && inStream >> tempReadStr) {
		if (tempReadStr.compare(NAME_KEYWORD) == 0) {
			if (std::getline(inStream, tempReadStr)) {
				tempReadStr = stringhelper::trim(tempReadStr);
				this->name = tempReadStr;
			}
			else {
				success = false;
				debug_output("Failed to read name for blammopedia entry: " << this->filename);
				break;
			}
		}
		else if (tempReadStr.compare(DESCRIPTION_KEYWORD) == 0) {
			if (std::getline(inStream, tempReadStr)) {
				tempReadStr = stringhelper::trim(tempReadStr);
				this->description = tempReadStr;
			}
			else {
				success = false;
				debug_output("Failed to read description for blammopedia entry: " << this->filename);
				break;
			}
		}
        else if (tempReadStr.compare(FINEPRINT_KEYWORD) == 0) {
			if (std::getline(inStream, tempReadStr)) {
				tempReadStr = stringhelper::trim(tempReadStr);
				this->finePrint = tempReadStr;
			}
			else {
				success = false;
				debug_output("Failed to read fineprint for blammopedia entry: " << this->filename);
				break;
			}
        }
	}
	return success;
}

Blammopedia::ItemEntry::~ItemEntry() {
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->itemTexture);
	assert(success);
	if (this->hudOutlineTexture != NULL) {
		success = ResourceManager::GetInstance()->ReleaseTextureResource(this->hudOutlineTexture);
		assert(success);
	}
	if (this->hudFillTexture != NULL) {
		success = ResourceManager::GetInstance()->ReleaseTextureResource(this->hudFillTexture);
		assert(success);
	}
}

// Populates this blammopedia item entry from its known resource file name
bool Blammopedia::ItemEntry::PopulateFromFile() {
	long fileLength;
	char* fileBuffer = ResourceManager::GetInstance()->FilepathToMemoryBuffer(this->filename, fileLength);
	if (fileBuffer == NULL || fileLength <= 0) {
		assert(false);
		return NULL;
	}
	std::istringstream strStream(std::string(fileBuffer), std::ios_base::in | std::ios_base::binary);
	delete[] fileBuffer;
	fileBuffer = NULL;

	bool success = Entry::PopulateBaseValuesFromStream(strStream);
	if (!success) {
		return false;
	}

	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);

	strStream.clear();
	strStream.seekg(0, std::ios::beg);

	std::string tempReadStr;
	while (success && strStream >> tempReadStr) {
		if (tempReadStr.compare(ITEM_TEXTURE_KEYWORD) == 0) {
			if (std::getline(strStream, tempReadStr)) {
				tempReadStr = stringhelper::trim(tempReadStr);
				this->itemTextureFilename = ResourceManager::GetTextureResourceDir() + tempReadStr;

				// Read the texture from the resource manager...
				this->itemTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(this->itemTextureFilename, Texture::Trilinear, GL_TEXTURE_2D));
				if (this->itemTexture == NULL) {
					assert(false);
					success = false;
					debug_output("Failed to load the item texture for blammopedia item entry: " << this->filename);
					break;
				}

			}
			else {
				success = false;
				debug_output("Failed to read the item texture for blammopedia item entry: " << this->filename);
				break;
			}
		}
		else if (tempReadStr.compare(HUD_OUTLINE_TEXTURE_KEYWORD) == 0) {
			if (std::getline(strStream, tempReadStr)) {
				tempReadStr = stringhelper::trim(tempReadStr);
				this->hudOutlineTextureFilename = ResourceManager::GetTextureResourceDir() + tempReadStr;
			
				// Read the texture from the resource manager...
				this->hudOutlineTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(this->hudOutlineTextureFilename, 
																													 Texture::Trilinear, GL_TEXTURE_2D));
				if (this->hudOutlineTexture == NULL) {
					assert(false);
					success = false;
					debug_output("Failed to load the item HUD outline texture for blammopedia item entry: " << this->filename);
					break;
				}
				else {
					glBindTexture(GL_TEXTURE_2D, this->hudOutlineTexture->GetTextureID());
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

			}
			else {
				success = false;
				debug_output("Failed to read the item HUD outline texture for blammopedia item entry: " << this->filename);
				break;
			}
		}
		else if (tempReadStr.compare(HUD_FILL_TEXTURE_KEYWORD) == 0) {
			if (std::getline(strStream, tempReadStr)) {
				tempReadStr = stringhelper::trim(tempReadStr);
				this->hudFillTextureFilename = ResourceManager::GetTextureResourceDir() + tempReadStr;

				// Read the texture from the resource manager...
				this->hudFillTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(this->hudFillTextureFilename, 
																												Texture::Trilinear, GL_TEXTURE_2D));
				if (this->hudFillTexture == NULL) {
					assert(false);
					success = false;
					debug_output("Failed to load the item HUD fill texture for blammopedia item entry: " << this->filename);
					break;
				}
				else {
					glBindTexture(GL_TEXTURE_2D, this->hudFillTexture->GetTextureID());
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}
			}
			else {
				success = false;
				debug_output("Failed to read the item HUD fill texture for blammopedia item entry: " << this->filename);
				break;
			}
		}
	}

	glPopAttrib();

	return success;
}

Blammopedia::BlockEntry::~BlockEntry() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->blockTexture);
    UNUSED_VARIABLE(success);
	assert(success);
}

bool Blammopedia::BlockEntry::PopulateFromFile() {
	long fileLength;
	char* fileBuffer = ResourceManager::GetInstance()->FilepathToMemoryBuffer(this->filename, fileLength);
	if (fileBuffer == NULL || fileLength <= 0) {
		assert(false);
		return NULL;
	}
	std::istringstream strStream(std::string(fileBuffer), std::ios_base::in | std::ios_base::binary);
	delete[] fileBuffer;
	fileBuffer = NULL;

	bool success = Entry::PopulateBaseValuesFromStream(strStream);
	if (!success) {
		return false;
	}

	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);

	strStream.clear();
	strStream.seekg(0, std::ios::beg);

    std::string tempReadStr;
	while (success && strStream >> tempReadStr) {
		if (tempReadStr.compare(DISPLAY_TEXTURE_KEYWORD) == 0) {
			if (std::getline(strStream, tempReadStr)) {
				tempReadStr = stringhelper::trim(tempReadStr);
                this->blockTextureFilename = ResourceManager::GetTextureResourceDir() + tempReadStr;

				// Read the texture from the resource manager...
				this->blockTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(this->blockTextureFilename, Texture::Trilinear, GL_TEXTURE_2D));
				if (this->blockTexture == NULL) {
					assert(false);
					success = false;
					debug_output("Failed to load the block display texture for blammopedia item entry: " << this->filename);
					break;
				}
			}
			else {
				success = false;
				debug_output("Failed to read the item texture for blammopedia item entry: " << this->filename);
				break;
			}
		}
	}
	glPopAttrib();

	return success;
}

Blammopedia::SolidBlockEntry::SolidBlockEntry(const std::string& filename) : AbstractBlockEntry(filename) {

}

Blammopedia::SolidBlockEntry::~SolidBlockEntry() {
    for (std::map<GameWorld::WorldStyle, Texture2D*>::iterator iter = this->blockTextureMap.begin(); 
        iter != this->blockTextureMap.end(); ++iter) {

            bool success = ResourceManager::GetInstance()->ReleaseTextureResource(iter->second);
            UNUSED_VARIABLE(success);
            assert(success);

    }

    this->blockTextureMap.clear();
    this->blockTextureFilenameMap.clear();
}

Texture2D* Blammopedia::SolidBlockEntry::GetBlockTexture(int furthestWorldIdx) const {
    assert(furthestWorldIdx+1 <= static_cast<int>(this->blockTextureMap.size()));

    int randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % (furthestWorldIdx+1);
    assert(randomIdx >= 0 && randomIdx <= furthestWorldIdx);

    std::map<GameWorld::WorldStyle, Texture2D*>::const_iterator findIter =
        this->blockTextureMap.find(static_cast<GameWorld::WorldStyle>(randomIdx));
    assert(findIter != this->blockTextureMap.end());
    return findIter->second;
};

bool Blammopedia::SolidBlockEntry::PopulateFromFile() {
    
    long fileLength;
    char* fileBuffer = ResourceManager::GetInstance()->FilepathToMemoryBuffer(this->filename, fileLength);
    if (fileBuffer == NULL || fileLength <= 0) {
        assert(false);
        return NULL;
    }

    std::istringstream strStream(std::string(fileBuffer), std::ios_base::in | std::ios_base::binary);
    delete[] fileBuffer;
    fileBuffer = NULL;

    if (!Entry::PopulateBaseValuesFromStream(strStream)) {
        return false;
    }

    glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);

    GameViewConstants::GetInstance()->GetWorldTypeBlockTextureNameMap(this->blockTextureFilenameMap);
    for (std::map<GameWorld::WorldStyle, const char*>::const_iterator iter = this->blockTextureFilenameMap.begin();
        iter != this->blockTextureFilenameMap.end(); ++iter) {

        this->blockTextureMap.insert(std::make_pair(iter->first, 
            static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
            iter->second, Texture::Trilinear, GL_TEXTURE_2D))));
    }

    glPopAttrib();

    return true;
}
