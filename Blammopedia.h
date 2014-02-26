/**
 * Blammopedia.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __BLAMMOPEDIA_H__
#define __BLAMMOPEDIA_H__

#include "BlammoEngine/BasicIncludes.h"

#include "GameModel/GameItem.h"
#include "GameModel/LevelPiece.h"
#include "GameModel/GameWorld.h"

#include <string>
#include <map>

class Texture2D;

class Blammopedia {

public:
	class Entry {
		friend class Blammopedia;
	public:
		Entry(const std::string& filename): filename(filename), isLocked(true), hasBeenViewed(false) {}
		virtual ~Entry() {};

		void SetIsLocked(bool locked) { this->isLocked = locked; }
		bool GetIsLocked() const { return this->isLocked; }

        void SetHasBeenViewed(bool viewed) { this->hasBeenViewed = viewed; }
        bool GetHasBeenViewed() const { return this->hasBeenViewed; }

		const std::string& GetName() const { return this->name; }
		const std::string& GetDescription() const { return this->description; }
        const std::string& GetFinePrint() const { return this->finePrint; }

	protected:
		// Item, block and status effect file keywords/syntax constants
		static const char* NAME_KEYWORD;
		static const char* ITEM_TEXTURE_KEYWORD;
		static const char* HUD_OUTLINE_TEXTURE_KEYWORD;
		static const char* HUD_FILL_TEXTURE_KEYWORD;
		static const char* DESCRIPTION_KEYWORD;
        static const char* FINEPRINT_KEYWORD;
        static const char* DISPLAY_TEXTURE_KEYWORD;

		std::string filename;

		bool isLocked;
        bool hasBeenViewed;
		std::string name;
		std::string description;
        std::string finePrint;

		virtual bool PopulateFromFile() = 0;
		bool PopulateBaseValuesFromStream(std::istream& inStream);
	};

	class ItemEntry : public Entry {
		friend class Blammopedia;
	public:
		ItemEntry(const std::string& filename) : Entry(filename), 
			itemTexture(NULL), hudOutlineTexture(NULL), hudFillTexture(NULL) {};
		~ItemEntry();

		Texture2D* GetItemTexture() const { return this->itemTexture; }
		Texture2D* GetHUDOutlineTexture() const { return this->hudOutlineTexture; }
		Texture2D* GetHUDFillTexture() const { return this->hudFillTexture; }

	protected:
		bool PopulateFromFile();

	private:
		std::string itemTextureFilename;
		std::string hudOutlineTextureFilename;
		std::string hudFillTextureFilename;
		Texture2D* itemTexture;				// The item texture (texture that is shown when the item drops)
		Texture2D* hudOutlineTexture;
		Texture2D* hudFillTexture;
	};


	class AbstractBlockEntry : public Entry {
		friend class Blammopedia;
	public:
		AbstractBlockEntry(const std::string& filename) : Entry(filename) {}
        virtual ~AbstractBlockEntry() {}

        virtual Texture2D* GetBlockTexture(int furthestWorldIdx) const = 0;
	};

    class BlockEntry : public AbstractBlockEntry {
        friend class Blammopedia;
    public:
        BlockEntry(const std::string& filename) : AbstractBlockEntry(filename), blockTexture(NULL) {}
        ~BlockEntry();

        Texture2D* GetBlockTexture(int) const { return this->blockTexture; }

    private:
        bool PopulateFromFile();

        std::string blockTextureFilename;
        Texture2D* blockTexture;
    };


    class SolidBlockEntry : public AbstractBlockEntry {
        friend class Blammopedia;
    public:
        SolidBlockEntry(const std::string& filename);
        ~SolidBlockEntry();
        
        Texture2D* GetBlockTexture(int furthestWorldIdx) const;

    private:
        bool PopulateFromFile();

        std::map<GameWorld::WorldStyle, const char*> blockTextureFilenameMap;
        std::map<GameWorld::WorldStyle, Texture2D*> blockTextureMap;
    };

	class MiscEntry : public Entry {
        friend class Blammopedia;
	public:
		MiscEntry(const std::string& filename) : Entry(filename) {}
		~MiscEntry() {};
	};

	typedef std::map<GameItem::ItemType, Blammopedia::ItemEntry*> ItemEntryMap;
	typedef std::map<LevelPiece::LevelPieceType, Blammopedia::AbstractBlockEntry*> BlockEntryMap;
	typedef std::map<LevelPiece::PieceStatus, Blammopedia::MiscEntry*> MiscEntryMap;

	typedef ItemEntryMap::iterator	ItemEntryMapIter;
	typedef BlockEntryMap::iterator	BlockEntryMapIter;
	typedef MiscEntryMap::iterator  MiscEntryMapIter;

	typedef ItemEntryMap::const_iterator   ItemEntryMapConstIter;
	typedef BlockEntryMap::const_iterator  BlockEntryMapConstIter;
	typedef MiscEntryMap::const_iterator   MiscEntryMapConstIter;

	~Blammopedia();
	
	Blammopedia::ItemEntry*  GetItemEntry(const GameItem::ItemType& itemType) const;
	Blammopedia::AbstractBlockEntry* GetBlockEntry(const LevelPiece::LevelPieceType& blockType) const;
	Blammopedia::MiscEntry*  GetMiscEntry(const LevelPiece::PieceStatus& statusType) const;

    void UnlockItem(const GameItem::ItemType& itemType);
    void UnlockBlock(const LevelPiece::LevelPieceType& blockType);

	const Blammopedia::ItemEntryMap& GetItemEntries() const;
	const Blammopedia::BlockEntryMap& GetBlockEntries() const;
	const Blammopedia::MiscEntryMap& GetMiscEntries() const;

    bool HasUnviewed() const;
    bool HasUnviewedBlocks() const;
    bool HasUnviewedItems() const;

    void ClearProgressData();

    static Blammopedia* BuildFromBlammopediaFile(const std::string &filepath);
	bool WriteAsEntryStatusFile() const;

	const Texture2D* GetLockedItemTexture() const;

private:
    std::string blammopediaFile;

	ItemEntryMap itemEntries;
	BlockEntryMap blockEntries;
	MiscEntryMap miscEntries;

	Texture2D* lockedItemTexture;

	Blammopedia();
	bool InitializeEntries();

	static bool ReadItemEntires(std::istream& inStream, std::map<GameItem::ItemType, std::pair<bool, bool> >& itemStatusMap);
	static bool ReadBlockEntries(std::istream& inStream, std::map<LevelPiece::LevelPieceType, std::pair<bool, bool> >& blockStatusMap);
	//static bool ReadMiscEntries(std::istream& inStream, std::map<LevelPiece::PieceStatus, bool>& miscStatusMap);
		
	DISALLOW_COPY_AND_ASSIGN(Blammopedia);
};

inline Blammopedia::ItemEntry* Blammopedia::GetItemEntry(const GameItem::ItemType& itemType) const {
	std::map<GameItem::ItemType, Blammopedia::ItemEntry*>::const_iterator findIter = this->itemEntries.find(itemType);
	if (findIter == this->itemEntries.end()) {
		assert(false);
		return NULL;
	}
		
	return findIter->second;
}

inline Blammopedia::AbstractBlockEntry* Blammopedia::GetBlockEntry(const LevelPiece::LevelPieceType& blockType) const {
    if (blockType == LevelPiece::Empty) {
        return NULL;
    }
	std::map<LevelPiece::LevelPieceType, Blammopedia::AbstractBlockEntry*>::const_iterator findIter = this->blockEntries.find(blockType);
	if (findIter == this->blockEntries.end()) {
		assert(false);
		return NULL;
	}
		
	return findIter->second;
}

inline Blammopedia::MiscEntry* Blammopedia::GetMiscEntry(const LevelPiece::PieceStatus& statusType) const {
	std::map<LevelPiece::PieceStatus, Blammopedia::MiscEntry*>::const_iterator findIter = this->miscEntries.find(statusType);
	if (findIter == this->miscEntries.end()) {
		assert(false);
		return NULL;
	}
		
	return findIter->second;
}

inline void Blammopedia::UnlockItem(const GameItem::ItemType& itemType) {
    Blammopedia::ItemEntry* itemEntry = this->GetItemEntry(itemType);
    assert(itemEntry != NULL);
    itemEntry->SetIsLocked(false);
}

inline void Blammopedia::UnlockBlock(const LevelPiece::LevelPieceType& blockType) {
    Blammopedia::AbstractBlockEntry* blockEntry = this->GetBlockEntry(blockType);
    if (blockEntry == NULL) {
        return;
    }
    blockEntry->SetIsLocked(false);
}

inline const Blammopedia::ItemEntryMap& Blammopedia::GetItemEntries() const {
	return this->itemEntries;
}

inline const Blammopedia::BlockEntryMap& Blammopedia::GetBlockEntries() const {
	return this->blockEntries;
}

inline const Blammopedia::MiscEntryMap& Blammopedia::GetMiscEntries() const {
	return this->miscEntries;
}

inline const Texture2D* Blammopedia::GetLockedItemTexture() const {
	return this->lockedItemTexture;
}

#endif // __BLAMMOPEDIAENTRY_H__